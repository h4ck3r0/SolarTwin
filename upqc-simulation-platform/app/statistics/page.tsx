'use client';

import React, { useState, useEffect } from 'react';
import {
  ResponsiveContainer,
  LineChart,
  Line,
  XAxis,
  YAxis,
  CartesianGrid,
  Tooltip,
  AreaChart,
  Area,
} from 'recharts';
import { SimulationDataPoint } from '@/lib/simulation-types';
import Link from 'next/link';
import { ArrowLeft, Activity, Download, ChevronRight } from 'lucide-react';

export default function StatisticsPage() {
  const [data, setData] = useState<SimulationDataPoint[]>([]);
  const [isLoading, setIsLoading] = useState(true);

  useEffect(() => {
    // Load simulation results from localStorage
    const stored = localStorage.getItem('simulation_results');
    if (stored) {
      try {
        const parsed = JSON.parse(stored);
        if (Array.isArray(parsed) && parsed.length > 0) {
          setData(parsed);
        }
      } catch (e) {
        console.error("Failed to parse simulation results", e);
      }
    }
    setIsLoading(false);
  }, []);

  const handleExportCSV = () => {
    if (data.length === 0) return;
    const headers = Object.keys(data[0]);
    const rows = data.map((dp) => headers.map((k) => (dp as any)[k]));
    const csvContent = 'data:text/csv;charset=utf-8,' + [headers.join(','), ...rows.map((e) => e.join(','))].join('\n');
    const link = document.createElement('a');
    link.setAttribute('href', encodeURI(csvContent));
    link.setAttribute('download', 'simulation_full_telemetry.csv');
    document.body.appendChild(link);
    link.click();
    document.body.removeChild(link);
  };

  if (isLoading) {
    return (
      <div className="min-h-screen bg-slate-50 flex items-center justify-center">
        <div className="animate-pulse text-sky-600 font-mono">Loading telemetry...</div>
      </div>
    );
  }

  if (data.length === 0) {
    return (
      <div className="min-h-screen bg-slate-50 flex flex-col items-center justify-center font-mono">
        <Activity className="w-12 h-12 text-slate-300 mb-4" />
        <h2 className="text-xl font-bold text-slate-700">No Simulation Data Found</h2>
        <p className="text-slate-500 mb-6">Please run a simulation in the Workspace first.</p>
        <Link href="/" className="px-4 py-2 bg-sky-600 hover:bg-sky-500 text-white rounded font-bold transition-colors">
          Go to Workspace
        </Link>
      </div>
    );
  }

  const renderSinglePhaseChart = (title: string, dataKey: string, color: string, unit: string) => (
    <div className="bg-white border border-slate-200 rounded-lg p-4 shadow-sm flex flex-col h-64">
      <div className="text-xs font-bold text-slate-600 uppercase tracking-wider mb-2">{title}</div>
      <div className="flex-1 w-full">
        <ResponsiveContainer width="100%" height="100%">
          <AreaChart data={data} margin={{ top: 5, right: 5, left: 0, bottom: 0 }}>
            <defs>
              <linearGradient id={`grad-${dataKey}`} x1="0" y1="0" x2="0" y2="1">
                <stop offset="5%" stopColor={color} stopOpacity={0.3} />
                <stop offset="95%" stopColor={color} stopOpacity={0.0} />
              </linearGradient>
            </defs>
            <CartesianGrid strokeDasharray="3 3" stroke="#f1f5f9" vertical={true} />
            <XAxis dataKey="time" tickFormatter={(t) => `${(t * 1000).toFixed(0)}`} stroke="#94a3b8" style={{ fontSize: 9, fontFamily: 'monospace' }} />
            <YAxis stroke="#94a3b8" style={{ fontSize: 9, fontFamily: 'monospace' }} domain={['auto', 'auto']} unit={` ${unit}`} />
            <Tooltip
              contentStyle={{ backgroundColor: '#ffffff', borderColor: '#e2e8f0', borderRadius: '6px' }}
              labelStyle={{ fontSize: 10, color: '#0ea5e9', fontFamily: 'monospace', fontWeight: 'bold' }}
              itemStyle={{ fontSize: 10, fontFamily: 'monospace' }}
              labelFormatter={(lbl) => `Time: ${(lbl * 1000).toFixed(2)} ms`}
            />
            <Area type="monotone" dataKey={dataKey} stroke={color} strokeWidth={2} fillOpacity={1} fill={`url(#grad-${dataKey})`} isAnimationActive={false} />
          </AreaChart>
        </ResponsiveContainer>
      </div>
    </div>
  );

  const renderThreePhaseChart = (title: string, keys: string[], colors: string[], unit: string) => (
    <div className="bg-white border border-slate-200 rounded-lg p-4 shadow-sm flex flex-col h-64">
      <div className="text-xs font-bold text-slate-600 uppercase tracking-wider mb-2">{title}</div>
      <div className="flex-1 w-full">
        <ResponsiveContainer width="100%" height="100%">
          <LineChart data={data} margin={{ top: 5, right: 5, left: 0, bottom: 0 }}>
            <CartesianGrid strokeDasharray="3 3" stroke="#f1f5f9" vertical={true} />
            <XAxis dataKey="time" tickFormatter={(t) => `${(t * 1000).toFixed(0)}`} stroke="#94a3b8" style={{ fontSize: 9, fontFamily: 'monospace' }} />
            <YAxis stroke="#94a3b8" style={{ fontSize: 9, fontFamily: 'monospace' }} domain={['auto', 'auto']} unit={` ${unit}`} />
            <Tooltip
              contentStyle={{ backgroundColor: '#ffffff', borderColor: '#e2e8f0', borderRadius: '6px' }}
              labelStyle={{ fontSize: 10, color: '#0ea5e9', fontFamily: 'monospace', fontWeight: 'bold' }}
              itemStyle={{ fontSize: 10, fontFamily: 'monospace' }}
              labelFormatter={(lbl) => `Time: ${(lbl * 1000).toFixed(2)} ms`}
            />
            {keys.map((k, i) => (
              <Line key={k} type="monotone" dataKey={k} stroke={colors[i]} strokeWidth={1.5} dot={false} isAnimationActive={false} />
            ))}
          </LineChart>
        </ResponsiveContainer>
      </div>
    </div>
  );

  return (
    <div className="min-h-screen bg-slate-50 text-slate-900 p-4 md:p-8 font-mono select-none flex flex-col">
      {/* Header */}
      <div className="flex flex-col md:flex-row md:items-center justify-between gap-4 mb-6">
        <div>
          <Link href="/" className="inline-flex items-center text-sky-600 hover:text-sky-500 mb-2 transition-colors text-xs font-bold uppercase tracking-wider">
            <ArrowLeft className="w-3.5 h-3.5 mr-1" />
            Back to Workspace
          </Link>
          <h1 className="text-2xl font-bold text-slate-900 tracking-wide flex items-center gap-2">
            <Activity className="w-6 h-6 text-sky-600" />
            Simulation Analytics Dashboard
          </h1>
          <p className="text-slate-500 text-xs mt-1">
            Complete high-resolution telemetry from the Python EMT physics solver.
          </p>
        </div>

        <button
          onClick={handleExportCSV}
          className="flex items-center bg-white border border-slate-300 hover:border-sky-500 hover:text-sky-600 text-slate-700 px-4 py-2 rounded shadow-sm transition-all font-bold text-xs"
        >
          <Download className="w-4 h-4 mr-2" />
          Export All CSV
        </button>
      </div>

      {/* Grid of Charts */}
      <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-3 xl:grid-cols-4 gap-4 mb-8">
        {renderSinglePhaseChart("Solar PV Output", "solarPowerWatts", "#fbbf24", "W")}
        {renderSinglePhaseChart("Solar DC Voltage", "solarVoltageDc", "#eab308", "V")}
        {renderSinglePhaseChart("Solar DC Current", "solarCurrentDc", "#ca8a04", "A")}
        {renderSinglePhaseChart("Solar Irradiance", "solarIrradiance", "#f59e0b", "W/m²")}
        {renderSinglePhaseChart("Solar Temperature", "solarTemperature", "#ef4444", "°C")}
        {renderSinglePhaseChart("DC Link Voltage", "dcLinkVoltage", "#f97316", "V")}
        {renderSinglePhaseChart("Battery SOC", "batterySOC", "#10b981", "%")}
        {renderSinglePhaseChart("Wind Power", "windPowerWatts", "#06b6d4", "W")}
        {renderThreePhaseChart("Grid Voltage", ["gridVoltageA", "gridVoltageB", "gridVoltageC"], ["#E11D48", "#D97706", "#0284C7"], "V")}
        {renderThreePhaseChart("Grid Current", ["gridCurrentA", "gridCurrentB", "gridCurrentC"], ["#E11D48", "#D97706", "#0284C7"], "A")}
        {renderThreePhaseChart("Supply Voltage", ["supplyVoltageA", "supplyVoltageB", "supplyVoltageC"], ["#E11D48", "#D97706", "#0284C7"], "V")}
        {renderThreePhaseChart("Load Current", ["loadCurrentA", "loadCurrentB", "loadCurrentC"], ["#E11D48", "#D97706", "#0284C7"], "A")}
        {renderThreePhaseChart("Injecting Voltage (Series)", ["injectingVoltageA", "injectingVoltageB", "injectingVoltageC"], ["#E11D48", "#D97706", "#0284C7"], "V")}
        {renderThreePhaseChart("Injecting Current (Shunt)", ["injectingCurrentA", "injectingCurrentB", "injectingCurrentC"], ["#E11D48", "#D97706", "#0284C7"], "A")}
      </div>

      {/* Raw Data Table */}
      <div className="bg-white border border-slate-200 rounded-lg shadow-sm flex flex-col overflow-hidden">
        <div className="px-4 py-3 border-b border-slate-100 bg-slate-50 flex items-center justify-between">
          <div className="text-sm font-bold text-slate-700 flex items-center gap-2">
            <ChevronRight className="w-4 h-4 text-slate-400" />
            Raw Data Telemetry (Microsecond Resolution)
          </div>
          <div className="text-xs text-slate-500">{data.length} Sample Points</div>
        </div>
        <div className="w-full overflow-x-auto max-h-96">
          <table className="w-full text-left text-[10px] text-slate-600 border-collapse whitespace-nowrap">
            <thead className="bg-slate-50 sticky top-0 shadow-sm z-10">
              <tr>
                <th className="p-2 border-b border-slate-200 font-bold">Time (s)</th>
                <th className="p-2 border-b border-slate-200 font-bold">Grid V (A)</th>
                <th className="p-2 border-b border-slate-200 font-bold">Grid V (B)</th>
                <th className="p-2 border-b border-slate-200 font-bold">Grid V (C)</th>
                <th className="p-2 border-b border-slate-200 font-bold">Grid I (A)</th>
                <th className="p-2 border-b border-slate-200 font-bold">Grid I (B)</th>
                <th className="p-2 border-b border-slate-200 font-bold">Grid I (C)</th>
                <th className="p-2 border-b border-slate-200 font-bold">Load V (A)</th>
                <th className="p-2 border-b border-slate-200 font-bold">Load I (A)</th>
                <th className="p-2 border-b border-slate-200 font-bold">Inj V (A)</th>
                <th className="p-2 border-b border-slate-200 font-bold">Inj I (A)</th>
                <th className="p-2 border-b border-slate-200 font-bold">DC Link (V)</th>
                <th className="p-2 border-b border-slate-200 font-bold">Irrad (W/m²)</th>
                <th className="p-2 border-b border-slate-200 font-bold">Solar (W)</th>
              </tr>
            </thead>
            <tbody>
              {data.map((dp, i) => (
                <tr key={i} className="hover:bg-slate-100 border-b border-slate-100 transition-colors">
                  <td className="p-2 font-mono bg-slate-50">{dp.time.toFixed(4)}</td>
                  <td className="p-2">{dp.gridVoltageA.toFixed(1)}</td>
                  <td className="p-2">{dp.gridVoltageB.toFixed(1)}</td>
                  <td className="p-2">{dp.gridVoltageC.toFixed(1)}</td>
                  <td className="p-2">{dp.gridCurrentA.toFixed(1)}</td>
                  <td className="p-2">{dp.gridCurrentB.toFixed(1)}</td>
                  <td className="p-2">{dp.gridCurrentC.toFixed(1)}</td>
                  <td className="p-2">{dp.supplyVoltageA.toFixed(1)}</td>
                  <td className="p-2">{dp.loadCurrentA.toFixed(1)}</td>
                  <td className="p-2">{dp.injectingVoltageA.toFixed(1)}</td>
                  <td className="p-2">{dp.injectingCurrentA.toFixed(1)}</td>
                  <td className="p-2 font-bold text-orange-600">{dp.dcLinkVoltage.toFixed(1)}</td>
                  <td className="p-2 font-bold text-yellow-600">{dp.solarIrradiance.toFixed(0)}</td>
                  <td className="p-2 font-bold text-amber-600">{dp.solarPowerWatts.toFixed(0)}</td>
                </tr>
              ))}
            </tbody>
          </table>
        </div>
      </div>
    </div>
  );
}
