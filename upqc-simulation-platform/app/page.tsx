'use client';

import React, { useState, useEffect, useRef, useCallback } from 'react';
import { ReactFlowProvider } from 'reactflow';
import SimulationToolbar from '@/components/SimulationToolbar';
import NodeParameterModal from '@/components/NodeParameterModal';
import ModelExplorer from '@/components/ModelExplorer';
import SimulationCanvas from '@/components/SimulationCanvas';
import ParameterPanel from '@/components/ParameterPanel';
import SolarPVTelemetryCard from '@/components/SolarPVTelemetryCard';
import Link from 'next/link';
import { CheckCircle2, X } from 'lucide-react';
import { SimulationParameters, SimulationDataPoint, SimulationStatus } from '@/lib/simulation-types';

const DEFAULT_PARAMETERS: SimulationParameters = {
  microgridVoltage: 415,
  microgridFrequency: 50,
  solarIrradiance: 1000,
  solarTemperature: 25,
  solarStringsParallel: 10,
  solarModulesSeries: 7,
  solarPanelWatts: 415,
  solarVmpp: 34.1,
  batterySOC: 80,
  batteryCapacityKwh: 500,
  dcLinkVoltage: 700,
  windSpeed: 8.0,
  windCutIn: 3.0,
  windCutOut: 25.0,
  windNominalPower: 50.0,
  gridResistance: 0.01,
  gridReactance: 0.05,
  loadActivePower: 10.0,
  loadPowerFactor: 0.85,
  loadHarmonicType: 'Rectifier',
  filterInductance: 5.0,
  dcCapacitance: 50000,
  loadTHD: 28,
  kp: 5.0,
  ki: 50,
  isGridConnected: true,
  simulationDuration: 0.05,
};

export default function WorkspacePage() {
  const [status, setStatus] = useState<SimulationStatus>('IDLE');
  const [simulationTime, setSimulationTime] = useState<number>(0);
  const [selectedNodeId, setSelectedNodeId] = useState<string | null>(null);
  const [parameters, setParameters] = useState<SimulationParameters>({ ...DEFAULT_PARAMETERS });
  const [isClient, setIsClient] = useState(false);
  const [results, setResults] = useState<SimulationDataPoint[]>([]);
  const [resultsCollapsed, setResultsCollapsed] = useState<boolean>(false);
  const [editingNode, setEditingNode] = useState<{id: string, type: string, label: string} | null>(null);
  const [showSuccessBanner, setShowSuccessBanner] = useState(false);
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
    setIsClient(true);
    const saved = localStorage.getItem('solar_twin_params');
    if (saved) {
      try {
        setParameters(JSON.parse(saved));
      } catch(e) {}
    }
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
        setSimulationTime(finalSimTime);
        
        // Save to localStorage for statistics page
        localStorage.setItem('simulation_results', JSON.stringify(result.dataPoints));
        // FIX BUG-F01: Show success banner instead of auto-redirecting
        setShowSuccessBanner(true);
        
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

  // FIX BUG-F02: stable callback reference prevents stale closure in SimulationCanvas
  const handleTopologyChange = useCallback((ids: Set<string>) => {
    setConnectedNodeIds((prev) => {
      if (prev.size !== ids.size) return new Set(ids);
      for (const id of ids) {
        if (!prev.has(id)) return new Set(ids);
      }
      return prev;
    });
  }, []);

  const handleSelectNodeFromExplorer = (nodeId: string) => {
    setSelectedNodeId(nodeId);
  };

  return (
    <div className="h-full w-full flex flex-col bg-[#F8FAFC] text-slate-900 overflow-hidden font-mono select-none">
      {/* FIX BUG-F01: Success toast banner */}
      {showSuccessBanner && (
        <div className="absolute top-16 left-1/2 -translate-x-1/2 z-50 flex items-center gap-3 bg-emerald-900 border border-emerald-500/50 text-emerald-100 px-5 py-3 rounded-xl shadow-2xl shadow-emerald-900/50 animate-in">
          <CheckCircle2 className="w-5 h-5 text-emerald-400 flex-shrink-0" />
          <span className="font-bold text-sm">Simulation Complete!</span>
          <Link href="/statistics" className="bg-emerald-600 hover:bg-emerald-500 px-3 py-1 rounded-lg text-xs font-bold transition-colors">
            View Stats →
          </Link>
          <button onClick={() => setShowSuccessBanner(false)} className="text-emerald-400 hover:text-white ml-1">
            <X className="w-4 h-4" />
          </button>
        </div>
      )}
      {/* Top Navigation Toolbar */}
      <SimulationToolbar
        status={status}
        simulationTime={simulationTime}
        parameters={parameters}
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
            onTopologyChange={handleTopologyChange}
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
