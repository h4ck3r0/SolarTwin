'use client';

import React, { useState, useEffect, useRef } from 'react';
import { ReactFlowProvider } from 'reactflow';
import SimulationToolbar from '@/components/SimulationToolbar';
import ModelExplorer from '@/components/ModelExplorer';
import SimulationCanvas from '@/components/SimulationCanvas';
import ParameterPanel from '@/components/ParameterPanel';
import SimulationResults from '@/components/SimulationResults';
import SimulationStatusWidget from '@/components/SimulationStatus';
import { SimulationParameters, SimulationDataPoint, SimulationStatus } from '@/lib/simulation-types';

// Default UPQC baseline parameters
const DEFAULT_PARAMETERS: SimulationParameters = {
  gridVoltage: 415,
  gridFrequency: 50,
  gridRs: 0.1,
  gridLs: 0.001,
  solarIrradiance: 1000,
  windSpeed: 12,
  batterySOC: 80,
  dcLinkVoltage: 700,
  kpSeries: 1.5,
  kiSeries: 120,
  refVoltSeries: 415,
  kpShunt: 1.0,
  kiShunt: 85,
  refVoltShunt: 700,
};

export default function WorkspacePage() {
  const [status, setStatus] = useState<SimulationStatus>('IDLE');
  const [simulationTime, setSimulationTime] = useState<number>(0);
  const [selectedNodeId, setSelectedNodeId] = useState<string | null>(null);
  const [parameters, setParameters] = useState<SimulationParameters>({ ...DEFAULT_PARAMETERS });
  const [results, setResults] = useState<SimulationDataPoint[]>([]);
  const [resultsCollapsed, setResultsCollapsed] = useState<boolean>(true);

  // References to React Flow navigation methods inside SimulationCanvas
  const zoomInRef = useRef<(() => void) | null>(null);
  const zoomOutRef = useRef<(() => void) | null>(null);
  const fitViewRef = useRef<(() => void) | null>(null);

  // References to simulation timing increments
  const timerIntervalRef = useRef<NodeJS.Timeout | null>(null);
  const abortControllerRef = useRef<AbortController | null>(null);

  // Clean up timers on unmount
  useEffect(() => {
    return () => {
      if (timerIntervalRef.current) clearInterval(timerIntervalRef.current);
    };
  }, []);

  const handleRunSimulation = async () => {
    // Prevent double simulation runs
    if (status === 'RUNNING') return;

    // Reset previous run data
    setStatus('RUNNING');
    setSimulationTime(0);
    setResults([]);

    // Create abort controller for canceling the simulation request
    abortControllerRef.current = new AbortController();

    // Start solver timer animation (increments solver clock smoothly from 0 to 0.3s over 2.5 seconds)
    const runDurationMs = 2500;
    const finalSimTime = 0.3000;
    const incrementInterval = 50; // ms
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
        body: JSON.stringify(parameters),
        signal: abortControllerRef.current.signal,
      });

      if (!response.ok) {
        throw new Error('Simulation solver API error.');
      }

      const result = await response.json();

      if (result.success) {
        setResults(result.dataPoints);
        setStatus('COMPLETED');
        // Stop timer and set to exact final simulation time
        if (timerIntervalRef.current) clearInterval(timerIntervalRef.current);
        setSimulationTime(0.3000);
        // Expand the scope results drawer to highlight the results
        setResultsCollapsed(false);
      } else {
        throw new Error(result.message || 'Simulation execution failed.');
      }
    } catch (err: any) {
      if (err.name !== 'AbortError') {
        console.error(err);
        setStatus('FAILED');
        if (timerIntervalRef.current) clearInterval(timerIntervalRef.current);
      }
    }
  };

  const handleStopSimulation = () => {
    if (status !== 'RUNNING') return;

    if (abortControllerRef.current) {
      abortControllerRef.current.abort();
    }

    if (timerIntervalRef.current) {
      clearInterval(timerIntervalRef.current);
    }

    setStatus('IDLE');
  };

  const handleResetSimulation = () => {
    handleStopSimulation();
    setStatus('IDLE');
    setSimulationTime(0);
    setParameters({ ...DEFAULT_PARAMETERS });
    setResults([]);
    setResultsCollapsed(true);
    setSelectedNodeId(null);
  };

  const handleApplyParameters = (newParams: SimulationParameters) => {
    setParameters(newParams);
  };

  const handleResetParameters = () => {
    setParameters({ ...DEFAULT_PARAMETERS });
  };

  const handleSelectNodeFromExplorer = (nodeId: string) => {
    setSelectedNodeId(nodeId);
  };

  return (
    <div className="h-screen w-screen flex flex-col bg-slate-950 text-slate-100 overflow-hidden font-sans">
      {/* Top Toolbar */}
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
          />
        </ReactFlowProvider>

        {/* Right Parameters Editor */}
        <ParameterPanel
          selectedNodeId={selectedNodeId}
          globalParameters={parameters}
          onApply={handleApplyParameters}
          onReset={handleResetParameters}
        />
      </div>

      {/* Bottom Collapsible Scope Waveforms Results Drawer */}
      <SimulationResults
        dataPoints={results}
        isCollapsed={resultsCollapsed}
        onToggleCollapse={() => setResultsCollapsed((c) => !c)}
      />

      {/* Floating Simulation Solver Status Panel */}
      <SimulationStatusWidget status={status} />
    </div>
  );
}
