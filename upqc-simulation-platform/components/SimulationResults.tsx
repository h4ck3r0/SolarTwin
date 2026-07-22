'use client';

import React, { useState } from 'react';
import {
  ResponsiveContainer,
  AreaChart,
  Area,
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
  | 'solarPvPower'
  | 'gridVoltage'
  | 'gridCurrent'
  | 'supplyVoltage'
  | 'loadCurrent'
  | 'injectingVoltage'
  | 'injectingCurrent'
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
    id: 'solarPvPower',
    label: 'Solar PV Output',
    isThreePhase: false,
    unit: 'W',
    keys: ['solarPowerWatts'],
    colors: ['#fbbf24'],
  },
  {
    id: 'gridVoltage',
    label: 'Grid Voltage',
    isThreePhase: true,
    unit: 'V',
    keys: ['gridVoltageA', 'gridVoltageB', 'gridVoltageC'],
    colors: ['#f43f5e', '#fbbf24', '#00f0ff'],
  },
  {
    id: 'gridCurrent',
    label: 'Grid Current',
    isThreePhase: true,
    unit: 'A',
    keys: ['gridCurrentA', 'gridCurrentB', 'gridCurrentC'],
    colors: ['#f43f5e', '#fbbf24', '#00f0ff'],
  },
  {
    id: 'supplyVoltage',
    label: 'Supply/Load Voltage',
    isThreePhase: true,
    unit: 'V',
    keys: ['supplyVoltageA', 'supplyVoltageB', 'supplyVoltageC'],
    colors: ['#f43f5e', '#fbbf24', '#00f0ff'],
  },
  {
    id: 'loadCurrent',
    label: 'Load Current',
    isThreePhase: true,
    unit: 'A',
    keys: ['loadCurrentA', 'loadCurrentB', 'loadCurrentC'],
    colors: ['#f43f5e', '#fbbf24', '#00f0ff'],
  },
  {
    id: 'injectingVoltage',
    label: 'Injecting Voltage (Series)',
    isThreePhase: true,
    unit: 'V',
    keys: ['injectingVoltageA', 'injectingVoltageB', 'injectingVoltageC'],
    colors: ['#f43f5e', '#fbbf24', '#38bdf8'],
  },
  {
    id: 'injectingCurrent',
    label: 'Injecting Current (Shunt)',
    isThreePhase: true,
    unit: 'A',
    keys: ['injectingCurrentA', 'injectingCurrentB', 'injectingCurrentC'],
    colors: ['#f43f5e', '#fbbf24', '#38bdf8'],
  },
  {
    id: 'dcLinkVoltage',
    label: 'DC Link Voltage',
    isThreePhase: false,
    unit: 'V',
    keys: ['dcLinkVoltage'],
    colors: ['#f97316'],
  },
];

export default function SimulationResults({
  dataPoints,
  isCollapsed,
  onToggleCollapse,
}: SimulationResultsProps) {
  const [activeTab, setActiveTab] = useState<TabType>('solarPvPower');
  const [isMaximized, setIsMaximized] = useState(false);

  const activeTabConfig = TABS.find((t) => t.id === activeTab) || TABS[0];
  const displayData = dataPoints.length > 0 ? dataPoints : generateBaselineData();

  const handleExportCSV = () => {
    if (displayData.length === 0) return;
    const headers = ['Time (s)', ...activeTabConfig.keys];
    const rows = displayData.map((dp) => [
      dp.time,
      ...activeTabConfig.keys.map((k) => (dp as any)[k]),
    ]);
    const csvContent = 'data:text/csv;charset=utf-8,' + [headers.join(','), ...rows.map((e) => e.join(','))].join('\n');
    const link = document.createElement('a');
    link.setAttribute('href', encodeURI(csvContent));
    link.setAttribute('download', `simulation_${activeTabConfig.id}.csv`);
    document.body.appendChild(link);
    link.click();
    document.body.removeChild(link);
  };

  const renderChart = () => {
    // Single-phase signal gets bold glowing AreaChart with gradient fill
    if (!activeTabConfig.isThreePhase) {
      return (
        <ResponsiveContainer width="100%" height="100%">
          <AreaChart data={displayData} margin={{ top: 15, right: 25, left: 0, bottom: 5 }}>
            <defs>
              <linearGradient id="solarGrad" x1="0" y1="0" x2="0" y2="1">
                <stop offset="5%" stopColor={activeTabConfig.colors[0]} stopOpacity={0.4} />
                <stop offset="95%" stopColor={activeTabConfig.colors[0]} stopOpacity={0.0} />
              </linearGradient>
            </defs>
            <CartesianGrid strokeDasharray="3 3" stroke="#0e1f38" vertical={true} />
            <XAxis
              dataKey="time"
              tickFormatter={(t) => `${(t * 1000).toFixed(0)} ms`}
              stroke="#64748b"
              style={{ fontSize: 10, fontFamily: 'monospace' }}
            />
            <YAxis
              stroke="#64748b"
              style={{ fontSize: 10, fontFamily: 'monospace' }}
              domain={['auto', 'auto']}
              unit={` ${activeTabConfig.unit}`}
            />
            <Tooltip
              contentStyle={{ backgroundColor: '#060b13', borderColor: '#00f0ff', borderRadius: '6px', boxShadow: '0 0 10px rgba(0,240,255,0.2)' }}
              labelStyle={{ fontSize: 11, color: '#00f0ff', fontFamily: 'monospace', fontWeight: 'bold' }}
              itemStyle={{ fontSize: 11, color: '#fbbf24', fontFamily: 'monospace' }}
              labelFormatter={(lbl) => `Solver Time: ${(lbl * 1000).toFixed(2)} ms`}
            />
            <Area
              type="monotone"
              dataKey={activeTabConfig.keys[0]}
              name={activeTabConfig.label}
              stroke={activeTabConfig.colors[0]}
              strokeWidth={3}
              fillOpacity={1}
              fill="url(#solarGrad)"
              isAnimationActive={false}
            />
          </AreaChart>
        </ResponsiveContainer>
      );
    }

    // 3-Phase signals get high-contrast 3-line chart (Phase A, B, C)
    return (
      <ResponsiveContainer width="100%" height="100%">
        <LineChart data={displayData} margin={{ top: 15, right: 25, left: 0, bottom: 5 }}>
          <CartesianGrid strokeDasharray="3 3" stroke="#0e1f38" vertical={true} />
          <XAxis
            dataKey="time"
            tickFormatter={(t) => `${(t * 1000).toFixed(0)} ms`}
            stroke="#64748b"
            style={{ fontSize: 10, fontFamily: 'monospace' }}
          />
          <YAxis
            stroke="#64748b"
            style={{ fontSize: 10, fontFamily: 'monospace' }}
            domain={['auto', 'auto']}
            unit={` ${activeTabConfig.unit}`}
          />
          <Tooltip
            contentStyle={{ backgroundColor: '#060b13', borderColor: '#00f0ff', borderRadius: '6px' }}
            labelStyle={{ fontSize: 11, color: '#00f0ff', fontFamily: 'monospace', fontWeight: 'bold' }}
            itemStyle={{ fontSize: 11, fontFamily: 'monospace' }}
            labelFormatter={(lbl) => `Time: ${(lbl * 1000).toFixed(2)} ms`}
          />
          <Legend wrapperStyle={{ fontSize: 10, fontFamily: 'monospace', color: '#94a3b8' }} />
          {activeTabConfig.keys.map((key, idx) => {
            const phaseLabel = `Phase ${String.fromCharCode(65 + idx)}`;
            return (
              <Line
                key={key}
                type="monotone"
                dataKey={key}
                name={phaseLabel}
                stroke={activeTabConfig.colors[idx]}
                strokeWidth={2.5}
                dot={false}
                isAnimationActive={false}
              />
            );
          })}
        </LineChart>
      </ResponsiveContainer>
    );
  };

  return (
    <>
      <div
        className={`bg-[#060b13] border-t border-cyan-950/80 transition-all duration-300 flex flex-col select-none font-mono ${
          isCollapsed ? 'h-8' : 'h-[230px]'
        }`}
      >
        {/* Header Bar */}
        <div
          onClick={onToggleCollapse}
          className="h-8 bg-[#090f1a] border-b border-slate-850 px-3 flex items-center justify-between cursor-pointer"
        >
          <div className="flex items-center space-x-3">
            <span className="text-[10px] font-bold text-cyan-400 uppercase tracking-widest flex items-center space-x-1.5">
              <span className="w-2 h-2 rounded-full bg-cyan-400 animate-pulse"></span>
              <span>Oscilloscope Scope Drawer</span>
            </span>
            <span className="bg-cyan-950/80 border border-cyan-800 text-cyan-300 text-[8px] font-bold px-1.5 py-0.5 rounded">
              301 SAMPLES READY
            </span>
          </div>

          {/* Horizontal Tabs */}
          <div className="flex items-center space-x-1 overflow-x-auto" onClick={(e) => e.stopPropagation()}>
            {TABS.map((tab) => (
              <button
                key={tab.id}
                onClick={() => setActiveTab(tab.id)}
                className={`px-2 py-0.5 rounded text-[9px] transition-colors border ${
                  activeTab === tab.id
                    ? 'bg-amber-500/20 text-amber-300 font-bold border-amber-500/60 shadow-sm shadow-amber-500/20'
                    : 'bg-[#060b13] text-slate-400 border-slate-800 hover:text-slate-200'
                }`}
              >
                {tab.label}
              </button>
            ))}
          </div>

          {/* Scope Controls */}
          <div className="flex items-center space-x-1" onClick={(e) => e.stopPropagation()}>
            <button onClick={handleExportCSV} className="p-1 text-slate-400 hover:text-cyan-300" title="Export Scope Data (CSV)">
              <Download className="w-3.5 h-3.5" />
            </button>
            <button onClick={() => setIsMaximized(true)} className="p-1 text-slate-400 hover:text-cyan-300" title="Maximize Scope Window">
              <Maximize2 className="w-3.5 h-3.5" />
            </button>
            <button onClick={onToggleCollapse} className="p-1 text-slate-400 hover:text-slate-200">
              {isCollapsed ? <ChevronUp className="w-3.5 h-3.5" /> : <ChevronDown className="w-3.5 h-3.5" />}
            </button>
          </div>
        </div>

        {/* High-Resolution Scope View Area */}
        {!isCollapsed && (
          <div className="flex-1 bg-[#040810] p-2.5 relative flex flex-col justify-between overflow-hidden">
            {/* Top Waveform Header Bar */}
            <div className="flex justify-between items-center text-[10px] font-mono border-b border-slate-900 pb-1 px-1">
              <span className="font-bold text-amber-400 flex items-center space-x-2">
                <span>CH1 — {activeTabConfig.label} ({activeTabConfig.unit})</span>
                <span className="text-[8px] bg-amber-500/20 text-amber-300 border border-amber-500/40 px-1 py-0.2 rounded font-mono">
                  LIVE WAVEFORM
                </span>
              </span>
              <span className="text-slate-400 text-[9px]">
                Timebase: <strong className="text-cyan-400">5ms/div</strong> | Solver: <strong className="text-emerald-400">Discrete Ode45 (dt=50µs)</strong>
              </span>
            </div>

            <div className="flex-1 w-full h-[170px] mt-1">{renderChart()}</div>
          </div>
        )}
      </div>

      {/* Maximized High-Res Scope Window */}
      {isMaximized && (
        <div className="fixed inset-0 bg-black/90 backdrop-blur-md z-50 flex items-center justify-center p-6 select-none font-mono">
          <div className="bg-[#060b13] border border-cyan-500/60 rounded-lg w-full max-w-6xl h-[85vh] flex flex-col overflow-hidden shadow-2xl">
            <div className="h-10 bg-[#090f1a] px-4 border-b border-cyan-950 flex items-center justify-between">
              <span className="text-xs font-bold text-cyan-400 uppercase tracking-widest flex items-center space-x-2">
                <Maximize2 className="w-4 h-4" />
                <span>High-Resolution Scope Analyzer — {activeTabConfig.label} ({activeTabConfig.unit})</span>
              </span>
              <button onClick={() => setIsMaximized(false)} className="p-1.5 text-slate-400 hover:text-rose-400">
                <Minimize2 className="w-4 h-4" />
              </button>
            </div>
            <div className="flex-1 p-6 bg-[#040810] flex flex-col">
              <div className="text-xs text-slate-400 mb-3 flex justify-between">
                <span>Signal: <strong className="text-amber-400">{activeTabConfig.label}</strong></span>
                <span>Fixed Step Solver: <strong className="text-emerald-400">Dormand-Prince (Ode45)</strong></span>
              </div>
              <div className="flex-1 w-full">{renderChart()}</div>
            </div>
          </div>
        </div>
      )}
    </>
  );
}

function generateBaselineData(): SimulationDataPoint[] {
  const pts: SimulationDataPoint[] = [];
  for (let i = 0; i <= 300; i++) {
    const t = i * (0.3 / 300);
    pts.push({
      time: parseFloat(t.toFixed(4)),
      gridVoltageA: Math.round(338 * Math.sin(2 * Math.PI * 50 * t) * 10) / 10,
      gridVoltageB: Math.round(338 * Math.sin(2 * Math.PI * 50 * t - 2.09) * 10) / 10,
      gridVoltageC: Math.round(338 * Math.sin(2 * Math.PI * 50 * t + 2.09) * 10) / 10,
      gridCurrentA: Math.round(20 * Math.sin(2 * Math.PI * 50 * t) * 100) / 100,
      gridCurrentB: Math.round(20 * Math.sin(2 * Math.PI * 50 * t - 2.09) * 100) / 100,
      gridCurrentC: Math.round(20 * Math.sin(2 * Math.PI * 50 * t + 2.09) * 100) / 100,
      supplyVoltageA: Math.round(338 * Math.sin(2 * Math.PI * 50 * t) * 10) / 10,
      supplyVoltageB: Math.round(338 * Math.sin(2 * Math.PI * 50 * t - 2.09) * 10) / 10,
      supplyVoltageC: Math.round(338 * Math.sin(2 * Math.PI * 50 * t + 2.09) * 10) / 10,
      loadCurrentA: Math.round(20 * Math.sin(2 * Math.PI * 50 * t) * 100) / 100,
      loadCurrentB: Math.round(20 * Math.sin(2 * Math.PI * 50 * t - 2.09) * 100) / 100,
      loadCurrentC: Math.round(20 * Math.sin(2 * Math.PI * 50 * t + 2.09) * 100) / 100,
      injectingVoltageA: 0,
      injectingVoltageB: 0,
      injectingVoltageC: 0,
      injectingCurrentA: 0,
      injectingCurrentB: 0,
      injectingCurrentC: 0,
      dcLinkVoltage: 700,
      solarPowerWatts: 1525,
      solarVoltageDc: 163.5,
      solarCurrentDc: 9.33,
    });
  }
  return pts;
}
