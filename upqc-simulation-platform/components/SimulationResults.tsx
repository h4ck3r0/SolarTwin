'use client';

import React, { useState } from 'react';
import {
  ResponsiveContainer,
  LineChart,
  Line,
  XAxis,
  YAxis,
  CartesianGrid,
  Tooltip,
  Legend,
} from 'recharts';
import { SimulationDataPoint } from '@/lib/simulation-types';
import { ChevronUp, ChevronDown, Maximize2, Minimize2, Download } from 'lucide-react';

interface SimulationResultsProps {
  dataPoints: SimulationDataPoint[];
  isCollapsed: boolean;
  onToggleCollapse: () => void;
}

type TabType =
  | 'supplyVoltage'
  | 'loadCurrent'
  | 'injectingVoltage'
  | 'injectingCurrent'
  | 'gridVoltage'
  | 'gridCurrent'
  | 'dcLinkVoltage';

interface TabConfig {
  id: TabType;
  label: string;
  isThreePhase: boolean;
  unit: string;
  keys: string[];
  colors: string[];
}

const TABS: TabConfig[] = [
  {
    id: 'gridVoltage',
    label: 'Grid Voltage',
    isThreePhase: true,
    unit: 'V',
    keys: ['gridVoltageA', 'gridVoltageB', 'gridVoltageC'],
    colors: ['#ef4444', '#eab308', '#3b82f6'], // Red, Yellow, Blue (Electrical standard)
  },
  {
    id: 'gridCurrent',
    label: 'Grid Current',
    isThreePhase: true,
    unit: 'A',
    keys: ['gridCurrentA', 'gridCurrentB', 'gridCurrentC'],
    colors: ['#ef4444', '#eab308', '#3b82f6'],
  },
  {
    id: 'supplyVoltage',
    label: 'Supply/Load Voltage',
    isThreePhase: true,
    unit: 'V',
    keys: ['supplyVoltageA', 'supplyVoltageB', 'supplyVoltageC'],
    colors: ['#ef4444', '#eab308', '#3b82f6'],
  },
  {
    id: 'loadCurrent',
    label: 'Load Current',
    isThreePhase: true,
    unit: 'A',
    keys: ['loadCurrentA', 'loadCurrentB', 'loadCurrentC'],
    colors: ['#ef4444', '#eab308', '#3b82f6'],
  },
  {
    id: 'injectingVoltage',
    label: 'Injecting Voltage (Series)',
    isThreePhase: true,
    unit: 'V',
    keys: ['injectingVoltageA', 'injectingVoltageB', 'injectingVoltageC'],
    colors: ['#f43f5e', '#fbbf24', '#60a5fa'], // slightly lighter variant
  },
  {
    id: 'injectingCurrent',
    label: 'Injecting Current (Shunt)',
    isThreePhase: true,
    unit: 'A',
    keys: ['injectingCurrentA', 'injectingCurrentB', 'injectingCurrentC'],
    colors: ['#f43f5e', '#fbbf24', '#60a5fa'],
  },
  {
    id: 'dcLinkVoltage',
    label: 'DC Link Voltage',
    isThreePhase: false,
    unit: 'V',
    keys: ['dcLinkVoltage'],
    colors: ['#f97316'], // Orange for DC Link
  },
];

export default function SimulationResults({
  dataPoints,
  isCollapsed,
  onToggleCollapse,
}: SimulationResultsProps) {
  const [activeTab, setActiveTab] = useState<TabType>('gridVoltage');
  const [isMaximized, setIsMaximized] = useState(false);

  const activeTabConfig = TABS.find((t) => t.id === activeTab) || TABS[0];

  const handleExportCSV = () => {
    if (dataPoints.length === 0) return;
    
    const headers = ['Time (s)', ...activeTabConfig.keys];
    const rows = dataPoints.map((dp) => [
      dp.time,
      ...activeTabConfig.keys.map((k) => (dp as any)[k]),
    ]);
    
    const csvContent =
      'data:text/csv;charset=utf-8,' +
      [headers.join(','), ...rows.map((e) => e.join(','))].join('\n');
      
    const encodedUri = encodeURI(csvContent);
    const link = document.createElement('a');
    link.setAttribute('href', encodedUri);
    link.setAttribute('download', `simulation_${activeTabConfig.id}.csv`);
    document.body.appendChild(link);
    link.click();
    document.body.removeChild(link);
  };

  const renderChart = (fullscreen = false) => {
    if (dataPoints.length === 0) {
      return (
        <div className="flex flex-col items-center justify-center h-full text-slate-500 text-xs font-mono space-y-2">
          <span>[SCOPE BUS DISCONNECTED]</span>
          <span className="text-[10px] text-slate-600">Run the simulation solver to stream time-series waveform data.</span>
        </div>
      );
    }

    return (
      <ResponsiveContainer width="100%" height="100%">
        <LineChart data={dataPoints} margin={{ top: 15, right: 20, left: -5, bottom: 5 }}>
          <CartesianGrid strokeDasharray="3 3" stroke="#1e293b" />
          <XAxis
            dataKey="time"
            tickFormatter={(tick) => `${(tick * 1000).toFixed(1)} ms`}
            stroke="#64748b"
            style={{ fontSize: 10, fontFamily: 'monospace' }}
            label={{ value: 'Simulation Time', position: 'insideBottomRight', offset: -5, fill: '#64748b', style: { fontSize: 10, fontFamily: 'monospace' } }}
          />
          <YAxis
            stroke="#64748b"
            style={{ fontSize: 10, fontFamily: 'monospace' }}
            label={{ value: `${activeTabConfig.label} (${activeTabConfig.unit})`, angle: -90, position: 'insideLeft', offset: 10, fill: '#64748b', style: { fontSize: 10, fontFamily: 'monospace' } }}
          />
          <Tooltip
            contentStyle={{ backgroundColor: '#0f172a', borderColor: '#334155', borderRadius: '4px' }}
            labelStyle={{ fontSize: 10, color: '#94a3b8', fontFamily: 'monospace' }}
            itemStyle={{ fontSize: 11, fontFamily: 'monospace' }}
            labelFormatter={(label) => `Time: ${(label * 1000).toFixed(3)} ms`}
          />
          <Legend wrapperStyle={{ fontSize: 10, fontFamily: 'monospace', color: '#94a3b8' }} />
          {activeTabConfig.keys.map((key, index) => {
            const phaseLabel = activeTabConfig.isThreePhase
              ? `Phase ${String.fromCharCode(65 + index)}`
              : 'V_dc';
            return (
              <Line
                key={key}
                type="monotone"
                dataKey={key}
                name={phaseLabel}
                stroke={activeTabConfig.colors[index]}
                strokeWidth={fullscreen ? 2 : 1.5}
                dot={false}
                activeDot={{ r: 4 }}
              />
            );
          })}
        </LineChart>
      </ResponsiveContainer>
    );
  };

  return (
    <>
      {/* Collapsible Drawer Container */}
      <div
        className={`bg-slate-950 border-t border-slate-800 transition-all duration-300 flex flex-col select-none ${
          isCollapsed ? 'h-8' : 'h-[320px]'
        }`}
      >
        {/* Top Header/Bar of Drawer */}
        <div
          onClick={onToggleCollapse}
          className="h-8 bg-slate-900 border-b border-slate-800 px-3 flex items-center justify-between cursor-pointer hover:bg-slate-850"
        >
          <div className="flex items-center space-x-2">
            <span className="text-[10px] font-bold text-slate-400 uppercase tracking-wider font-mono">
              Simulation Scope Array
            </span>
            {dataPoints.length > 0 && (
              <span className="bg-blue-900/60 border border-blue-800 text-blue-400 text-[8px] font-bold px-1.5 py-0.5 rounded font-mono uppercase">
                {dataPoints.length} Samples Ready
              </span>
            )}
          </div>

          <div className="flex items-center space-x-1" onClick={(e) => e.stopPropagation()}>
            {dataPoints.length > 0 && (
              <>
                <button
                  onClick={handleExportCSV}
                  className="p-1 rounded hover:bg-slate-800 text-slate-400 hover:text-slate-200 transition-colors"
                  title="Export Scope Data (CSV)"
                >
                  <Download className="w-3.5 h-3.5" />
                </button>
                <button
                  onClick={() => setIsMaximized(true)}
                  className="p-1 rounded hover:bg-slate-800 text-slate-400 hover:text-slate-200 transition-colors"
                  title="Maximize Graph"
                >
                  <Maximize2 className="w-3.5 h-3.5" />
                </button>
              </>
            )}
            <button
              onClick={onToggleCollapse}
              className="p-1 rounded hover:bg-slate-800 text-slate-400 hover:text-slate-200 transition-colors"
            >
              {isCollapsed ? <ChevronUp className="w-3.5 h-3.5" /> : <ChevronDown className="w-3.5 h-3.5" />}
            </button>
          </div>
        </div>

        {/* Tab Selection & Graph Display */}
        {!isCollapsed && (
          <div className="flex flex-1 overflow-hidden">
            {/* Tabs Sidebar */}
            <div className="w-48 border-r border-slate-900 bg-slate-950 overflow-y-auto flex flex-col p-1.5 space-y-0.5">
              {TABS.map((tab) => (
                <button
                  key={tab.id}
                  onClick={() => setActiveTab(tab.id)}
                  className={`w-full text-left px-2 py-1.5 rounded text-[11px] font-mono transition-colors ${
                    activeTab === tab.id
                      ? 'bg-slate-800 text-amber-400 font-bold border border-slate-700'
                      : 'text-slate-400 hover:bg-slate-900 hover:text-slate-200 border border-transparent'
                  }`}
                >
                  {tab.label}
                </button>
              ))}
            </div>

            {/* Scope Graph Canvas */}
            <div className="flex-1 bg-slate-950 p-3 overflow-hidden h-full flex flex-col">
              <div className="flex justify-between items-center text-[10px] font-mono text-slate-400 mb-2 border-b border-slate-900 pb-1.5 px-1">
                <span className="font-bold text-slate-300">{activeTabConfig.label} Waveform Analysis</span>
                <span>Base Frequency: 50 Hz | Time Frame: 300 ms</span>
              </div>
              <div className="flex-1 h-[210px] w-full">{renderChart(false)}</div>
            </div>
          </div>
        )}
      </div>

      {/* Maximized Zoom Modal Overlay */}
      {isMaximized && (
        <div className="fixed inset-0 bg-slate-950/90 backdrop-blur-sm z-50 flex items-center justify-center p-6 select-none">
          <div className="bg-slate-950 border border-slate-800 rounded-lg shadow-2xl w-full max-w-5xl h-[85vh] flex flex-col overflow-hidden">
            {/* Modal Header */}
            <div className="h-10 bg-slate-900 px-4 border-b border-slate-800 flex items-center justify-between">
              <div className="flex items-center space-x-3">
                <span className="text-xs font-bold text-amber-400 uppercase tracking-widest font-mono">
                  Advanced Scope Analyzer
                </span>
                <span className="text-[10px] text-slate-500 font-mono">| {activeTabConfig.label}</span>
              </div>
              <div className="flex items-center space-x-2">
                <button
                  onClick={handleExportCSV}
                  className="flex items-center space-x-1.5 px-3 py-1 bg-slate-800 hover:bg-slate-750 text-slate-200 border border-slate-700 rounded text-xs transition-colors"
                >
                  <Download className="w-3.5 h-3.5" />
                  <span className="font-mono">Export CSV</span>
                </button>
                <button
                  onClick={() => setIsMaximized(false)}
                  className="p-1.5 rounded hover:bg-slate-800 text-slate-400 hover:text-rose-500 transition-colors"
                  title="Close Fullscreen"
                >
                  <Minimize2 className="w-4 h-4" />
                </button>
              </div>
            </div>

            {/* Modal Content */}
            <div className="flex-1 p-6 flex flex-col bg-slate-950 overflow-hidden">
              <div className="flex justify-between items-center text-xs font-mono text-slate-400 mb-4 px-1">
                <span>Signal: {activeTabConfig.label} ({activeTabConfig.unit})</span>
                <span className="bg-slate-900 border border-slate-800 px-2 py-0.5 rounded text-[11px] text-slate-400">
                  Fixed timestep: 500µs | Solver: discrete
                </span>
              </div>
              <div className="flex-1 w-full">{renderChart(true)}</div>
            </div>

            {/* Modal Footer */}
            <div className="h-8 bg-slate-900 px-4 border-t border-slate-800 flex items-center justify-between text-[10px] text-slate-500 font-mono">
              <span>Status: OK | Signal Quality: Nominal</span>
              <span>UPQC Platform v1.1.0</span>
            </div>
          </div>
        </div>
      )}
    </>
  );
}
