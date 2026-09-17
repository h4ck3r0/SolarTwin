'use client';

import React, { useState, useEffect, useRef } from 'react';
import { useRouter } from 'next/navigation';
import { ReactFlowProvider } from 'reactflow';
import SimulationToolbar from '@/components/SimulationToolbar';
import NodeParameterModal from '@/components/NodeParameterModal';
import ModelExplorer from '@/components/ModelExplorer';
import SimulationCanvas from '@/components/SimulationCanvas';
import ParameterPanel from '@/components/ParameterPanel';
import SolarPVTelemetryCard from '@/components/SolarPVTelemetryCard';
import { SimulationParameters, SimulationDataPoint, SimulationStatus } from '@/lib/simulation-types';

const DEFAULT_PARAMETERS: SimulationParameters = {
  microgridVoltage: 415,
  microgridFrequency: 50,
  solarIrradiance: 1000,
  solarTemperature: 25,
  solarStringsParallel: 88,
  solarModulesSeries: 7,
  solarPanelWatts: 415,
  solarVmpp: 34.1,
  batterySOC: 80,
  batteryCapacityKwh: 100,
  dcLinkVoltage: 700,
  windSpeed: 8.0,
  windCutIn: 3.0,
  windCutOut: 25.0,
  windNominalPower: 50.0,
  gridResistance: 0.1,
  gridReactance: 0.2,
  loadActivePower: 15.0,
  loadPowerFactor: 0.85,
  loadHarmonicType: 'Rectifier',
  filterInductance: 2.5,
  dcCapacitance: 2200,
  loadTHD: 28,
  kp: 0.5,
  ki: 10,
};

export default function WorkspacePage() {
  const router = useRouter();
  const [status, setStatus] = useState<SimulationStatus>('IDLE');
  const [simulationTime, setSimulationTime] = useState<number>(0);
  const [selectedNodeId, setSelectedNodeId] = useState<string | null>(null);
  const [parameters, setParameters] = useState<SimulationParameters>(() => {
    if (typeof window !== 'undefined') {
      const saved = localStorage.getItem('solar_twin_params');
      if (saved) {
        try {
          return JSON.parse(saved);
        } catch(e) {}
      }
    }
    return { ...DEFAULT_PARAMETERS };
  });
  const [results, setResults] = useState<SimulationDataPoint[]>([]);
  const [resultsCollapsed, setResultsCollapsed] = useState<boolean>(false);
  const [editingNode, setEditingNode] = useState<{id: string, type: string, label: string} | null>(null);
  // Reactive connected node IDs — updated whenever canvas edges change
  const [connectedNodeIds, setConnectedNodeIds] = useState<Set<string>>(new Set());

  const zoomInRef = useRef<(() => void) | null>(null);
  const zoomOutRef = useRef<(() => void) | null>(null);
  const fitViewRef = useRef<(() => void) | null>(null);
  const getTopologyRef = useRef<(() => { nodes: any[]; edges: any[] }) | null>(null);
  const updateNodeRef = useRef<((id: string, data: any) => void) | null>(null);

  const timerIntervalRef = useRef<NodeJS.Timeout | null>(null);
  const abortControllerRef = useRef<AbortController | null>(null);

  useEffect(() => {
    return () => {
      if (timerIntervalRef.current) clearInterval(timerIntervalRef.current);
    };
  }, []);

  useEffect(() => {
    localStorage.setItem('solar_twin_params', JSON.stringify(parameters));
  }, [parameters]);

  const handleRunSimulation = async () => {
    if (status === 'RUNNING') return;

    setStatus('RUNNING');
    setSimulationTime(0);
    setResults([]);

    abortControllerRef.current = new AbortController();

    const finalSimTime = parameters.simulationDuration ?? 0.3;
    const runDurationMs = Math.max(2500, finalSimTime * 8000);
    const incrementInterval = 50;
    const timeStep = finalSimTime / (runDurationMs / incrementInterval);

    timerIntervalRef.current = setInterval(() => {
      setSimulationTime((prev) => {
        const next = prev + timeStep;
        return next >= finalSimTime ? finalSimTime : next;
      });
    }, incrementInterval);

    try {
      const response = await fetch('/api/simulation/run', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({
          parameters,
          topology: getTopologyRef.current?.() || { nodes: [], edges: [] }
        }),
        signal: abortControllerRef.current.signal,
      });

      if (!response.ok) {
        throw new Error('Simulation solver API error.');
      }

      const result = await response.json();

      if (result.success) {
        setResults(result.dataPoints);
        setStatus('COMPLETED');
        if (timerIntervalRef.current) clearInterval(timerIntervalRef.current);
        setSimulationTime(0.3000);
        
        // Save to localStorage and redirect
        localStorage.setItem('simulation_results', JSON.stringify(result.dataPoints));
        router.push('/statistics');
        
      } else {
        throw new Error(result.message || 'Simulation execution failed.');
      }
    } catch (err: any) {
      if (err.name !== 'AbortError') {
        console.error(err);
        setStatus('IDLE');
        alert("Simulation backend failed to respond. Please ensure the Python server is running.");
      }
    } finally {
      if (timerIntervalRef.current) clearInterval(timerIntervalRef.current);
    }
  };

  const handleStopSimulation = () => {
    if (status !== 'RUNNING') return;
    if (abortControllerRef.current) abortControllerRef.current.abort();
    if (timerIntervalRef.current) clearInterval(timerIntervalRef.current);
    setStatus('IDLE');
  };

  const handleResetSimulation = () => {
    handleStopSimulation();
    setStatus('IDLE');
    setSimulationTime(0);
    setParameters({ ...DEFAULT_PARAMETERS });
    setResults([]);
    setResultsCollapsed(false);
    setSelectedNodeId(null);
  };

  const handleApplyParameters = (newParams: SimulationParameters) => {
    setParameters(newParams);
  };

  const handleResetParameters = () => {
    setParameters({ ...DEFAULT_PARAMETERS });
    localStorage.setItem('solar_twin_topology', '');
    window.location.reload();
  };

  const handleSelectNodeFromExplorer = (nodeId: string) => {
    setSelectedNodeId(nodeId);
  };

  return (
    <div className="h-full w-full flex flex-col bg-[#F8FAFC] text-slate-900 overflow-hidden font-mono select-none">
      {/* Top Navigation Toolbar */}
      <SimulationToolbar
        status={status}
        simulationTime={simulationTime}
        onRun={handleRunSimulation}
        onStop={handleStopSimulation}
        onReset={handleResetSimulation}
        onZoomIn={() => zoomInRef.current?.()}
        onZoomOut={() => zoomOutRef.current?.()}
        onFitView={() => fitViewRef.current?.()}
      />

      {/* Dedicated Top Solar PV Telemetry Banner */}
      <div className="px-2 py-1 bg-[#F8FAFC]">
        <SolarPVTelemetryCard
          parameters={parameters}
          topology={getTopologyRef.current ? getTopologyRef.current() : null}
          onUpdateParameters={handleApplyParameters}
          onUpdateNode={(id, params) => {
            if (updateNodeRef.current) updateNodeRef.current(id, params);
            setParameters(p => ({...p, ...params}));
          }}
          isSimulating={status === 'RUNNING'}
        />
      </div>

      {/* Main Workspace Body */}
      <div className="flex-1 flex overflow-hidden relative">
        {/* Left Model Explorer */}
        <ModelExplorer
          selectedNodeId={selectedNodeId}
          onSelectItem={handleSelectNodeFromExplorer}
        />

        {/* Center Interactive Model Diagram */}
        <ReactFlowProvider>
          <SimulationCanvas
            isSimulating={status === 'RUNNING'}
            selectedNodeId={selectedNodeId}
            onSelectNode={setSelectedNodeId}
            setZoomInRef={zoomInRef}
            setZoomOutRef={zoomOutRef}
            setFitViewRef={fitViewRef}
            getTopologyRef={getTopologyRef}
            setUpdateNodeRef={updateNodeRef}
            onNodeDoubleClick={(e, node) => setEditingNode({ id: node.id, type: node.type, label: node.data?.label || '' })}
            onTopologyChange={(ids) => {
              setConnectedNodeIds((prev) => {
                if (prev.size !== ids.size) return new Set(ids);
                for (const id of ids) {
                  if (!prev.has(id)) return new Set(ids);
                }
                return prev; // No change, return exact same Set reference to avoid re-render
              });
            }}
          />
        </ReactFlowProvider>

        {/* Right Parameter Editor */}
        {editingNode && (
          <NodeParameterModal
            nodeId={editingNode.id}
            nodeType={editingNode.type}
            nodeLabel={editingNode.label}
            globalParameters={parameters}
            nodeParameters={getTopologyRef.current?.().nodes.find(n => n.id === editingNode.id)?.data?.parameters}
            onClose={() => setEditingNode(null)}
            onApply={(id, params) => {
              if (updateNodeRef.current) {
                updateNodeRef.current(id, params);
                setParameters(p => ({...p, ...params}));
              }
            }}
          />
        )}
        <ParameterPanel
          selectedNodeId={selectedNodeId}
          selectedNodeType={selectedNodeId ? getTopologyRef.current?.().nodes.find((n: any) => n.id === selectedNodeId)?.type : undefined}
          globalParameters={parameters}
          nodeParameters={selectedNodeId ? getTopologyRef.current?.().nodes.find((n: any) => n.id === selectedNodeId)?.data?.parameters : undefined}
          connectedNodeIds={connectedNodeIds}
          onApply={handleApplyParameters}
          onApplyNode={(id, params) => {
            if (updateNodeRef.current) {
               updateNodeRef.current(id, params);
               setParameters(p => ({...p, ...params})); // force ui refresh
            }
          }}
          onReset={handleResetParameters}
        />
      </div>
    </div>
  );
}
