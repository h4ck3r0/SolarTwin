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
  Legend,
  AreaChart,
  Area,
} from 'recharts';
import { SimulationDataPoint } from '@/lib/simulation-types';
import Link from 'next/link';
import { ArrowLeft, Play, Sun, Zap, Activity, Cpu, ShieldCheck } from 'lucide-react';

export default function StatisticsPage() {
  const [data, setData] = useState<SimulationDataPoint[]>([]);
  const [isLoading, setIsLoading] = useState(false);

  const handleRunAnalysis = async () => {
    setIsLoading(true);
    try {
      const response = await fetch('/api/simulation/run', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({
          microgridVoltage: 415,
          microgridFrequency: 60,
          solarIrradiance: 1000,
          solarStringsParallel: 88,
          solarModulesSeries: 7,
          solarPanelWatts: 415,
          windSpeed: 12,
          batterySOC: 80,
          dcLinkVoltage: 700,
          kpSeries: 1.5,
          kiSeries: 120,
          refVoltSeries: 415,
          kpShunt: 1.0,
          kiShunt: 85,
          refVoltShunt: 700,
        }),
      });

      if (response.ok) {
        const result = await response.json();
        if (result.success) {
          setData(result.dataPoints);
        }
      }
    } catch (err) {
      console.error(err);
    } finally {
      setIsLoading(false);
    }
  };

  useEffect(() => {
    // Automatically trigger run on initial page load for convenience
    handleRunAnalysis();
  }, []);

  return (
    <div className="min-h-screen bg-[#050912] text-slate-100 p-6 md:p-8 font-mono select-none flex flex-col">
      {/* Page Header */}
      <div className="flex flex-col md:flex-row md:items-center justify-between gap-4 mb-8">
        <div>
          <Link href="/" className="inline-flex items-center text-cyan-400 hover:text-cyan-300 mb-3 transition-colors text-sm">
            <ArrowLeft className="w-4 h-4 mr-2" />
            Back to Interactive Simulation
          </Link>
          <h1 className="text-2xl md:text-3xl font-bold text-white tracking-wide flex items-center gap-3">
            <Activity className="w-8 h-8 text-cyan-400" />
            Solar Microgrid Analytics Dashboard
          </h1>
          <p className="text-slate-400 text-xs md:text-sm mt-1">
            Dynamic step profiling of solar irradiance (1000 &rarr; 600 &rarr; 200 &rarr; 1000 W/m²) and multi-parameter system response.
          </p>
        </div>

        <button
          onClick={handleRunAnalysis}
          disabled={isLoading}
          className="self-start md:self-auto flex items-center bg-cyan-600 hover:bg-cyan-500 text-white px-5 py-2.5 rounded shadow-lg shadow-cyan-900/50 transition-all font-bold text-sm disabled:opacity-50"
        >
          {isLoading ? (
            <span className="animate-pulse">Running Solver...</span>
          ) : (
            <>
              <Play className="w-4 h-4 mr-2" fill="currentColor" />
              Re-Run Analysis
            </>
          )}
        </button>
      </div>

      {/* Grid of Square Cards */}
      <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-3 gap-6">
        {/* Card 1: Solar Irradiance Profile */}
        <div className="bg-[#0a1122] rounded-xl border border-amber-500/30 p-4 flex flex-col shadow-xl aspect-square">
          <div className="flex items-center justify-between mb-2">
            <h2 className="text-sm font-bold text-amber-400 flex items-center gap-2">
              <Sun className="w-4 h-4 text-amber-400" />
              Solar Irradiance Profile
            </h2>
            <span className="text-[10px] bg-amber-500/20 text-amber-300 border border-amber-500/40 px-1.5 py-0.5 rounded font-mono">
              W/m² Step
            </span>
          </div>
          <p className="text-[11px] text-slate-400 mb-3">Dynamically changing from 1000 &rarr; 600 &rarr; 200 &rarr; 1000 W/m²</p>
          
          <div className="flex-1 w-full relative">
            <ResponsiveContainer width="100%" height="100%">
              <AreaChart data={data} margin={{ top: 10, right: 10, left: -20, bottom: 0 }}>
                <defs>
                  <linearGradient id="irrGrad" x1="0" y1="0" x2="0" y2="1">
                    <stop offset="5%" stopColor="#fbbf24" stopOpacity={0.4} />
                    <stop offset="95%" stopColor="#fbbf24" stopOpacity={0.0} />
                  </linearGradient>
                </defs>
                <CartesianGrid strokeDasharray="3 3" stroke="#1a2b4c" vertical={false} />
                <XAxis dataKey="time" tickFormatter={(t) => `${(t * 1000).toFixed(0)}ms`} stroke="#64748b" style={{ fontSize: '10px' }} />
                <YAxis stroke="#fbbf24" domain={[0, 1200]} style={{ fontSize: '10px' }} />
                <Tooltip contentStyle={{ backgroundColor: '#060b13', borderColor: '#fbbf24', borderRadius: '6px', fontSize: '11px' }} />
                <Area type="stepAfter" dataKey="solarIrradiance" name="Irradiance (W/m²)" stroke="#fbbf24" strokeWidth={2} fill="url(#irrGrad)" isAnimationActive={false} />
              </AreaChart>
            </ResponsiveContainer>
          </div>
        </div>

        {/* Card 2: Solar PV Active Power Output */}
        <div className="bg-[#0a1122] rounded-xl border border-cyan-500/30 p-4 flex flex-col shadow-xl aspect-square">
          <div className="flex items-center justify-between mb-2">
            <h2 className="text-sm font-bold text-cyan-400 flex items-center gap-2">
              <Zap className="w-4 h-4 text-cyan-400" />
              Solar PV Active Power Output
            </h2>
            <span className="text-[10px] bg-cyan-500/20 text-cyan-300 border border-cyan-500/40 px-1.5 py-0.5 rounded font-mono">
              Watts
            </span>
          </div>
          <p className="text-[11px] text-slate-400 mb-3">Power output tracking irradiance profile (255.6kW Peak)</p>
          
          <div className="flex-1 w-full relative">
            <ResponsiveContainer width="100%" height="100%">
              <AreaChart data={data} margin={{ top: 10, right: 10, left: -20, bottom: 0 }}>
                <defs>
                  <linearGradient id="pwrGrad" x1="0" y1="0" x2="0" y2="1">
                    <stop offset="5%" stopColor="#38bdf8" stopOpacity={0.4} />
                    <stop offset="95%" stopColor="#38bdf8" stopOpacity={0.0} />
                  </linearGradient>
                </defs>
                <CartesianGrid strokeDasharray="3 3" stroke="#1a2b4c" vertical={false} />
                <XAxis dataKey="time" tickFormatter={(t) => `${(t * 1000).toFixed(0)}ms`} stroke="#64748b" style={{ fontSize: '10px' }} />
                <YAxis stroke="#38bdf8" domain={[0, 260000]} style={{ fontSize: '10px' }} />
                <Tooltip contentStyle={{ backgroundColor: '#060b13', borderColor: '#38bdf8', borderRadius: '6px', fontSize: '11px' }} />
                <Area type="monotone" dataKey="solarPowerWatts" name="PV Power (W)" stroke="#38bdf8" strokeWidth={2} fill="url(#pwrGrad)" isAnimationActive={false} />
              </AreaChart>
            </ResponsiveContainer>
          </div>
        </div>

        {/* Card 3: DC Link Voltage Stability */}
        <div className="bg-[#0a1122] rounded-xl border border-orange-500/30 p-4 flex flex-col shadow-xl aspect-square">
          <div className="flex items-center justify-between mb-2">
            <h2 className="text-sm font-bold text-orange-400 flex items-center gap-2">
              <Cpu className="w-4 h-4 text-orange-400" />
              DC Link Voltage (Vdc)
            </h2>
            <span className="text-[10px] bg-orange-500/20 text-orange-300 border border-orange-500/40 px-1.5 py-0.5 rounded font-mono">
              700V Target
            </span>
          </div>
          <p className="text-[11px] text-slate-400 mb-3">DC bus capacitor voltage under dynamic PV generation</p>
          
          <div className="flex-1 w-full relative">
            <ResponsiveContainer width="100%" height="100%">
              <LineChart data={data} margin={{ top: 10, right: 10, left: -20, bottom: 0 }}>
                <CartesianGrid strokeDasharray="3 3" stroke="#1a2b4c" vertical={false} />
                <XAxis dataKey="time" tickFormatter={(t) => `${(t * 1000).toFixed(0)}ms`} stroke="#64748b" style={{ fontSize: '10px' }} />
                <YAxis stroke="#f97316" domain={[500, 750]} style={{ fontSize: '10px' }} />
                <Tooltip contentStyle={{ backgroundColor: '#060b13', borderColor: '#f97316', borderRadius: '6px', fontSize: '11px' }} />
                <Line type="monotone" dataKey="dcLinkVoltage" name="DC Link Voltage (V)" stroke="#f97316" strokeWidth={2} dot={false} isAnimationActive={false} />
              </LineChart>
            </ResponsiveContainer>
          </div>
        </div>

        {/* Card 4: 3-Phase Grid Voltage */}
        <div className="bg-[#0a1122] rounded-xl border border-emerald-500/30 p-4 flex flex-col shadow-xl aspect-square">
          <div className="flex items-center justify-between mb-2">
            <h2 className="text-sm font-bold text-emerald-400 flex items-center gap-2">
              <Activity className="w-4 h-4 text-emerald-400" />
              3-Phase Microgrid Voltage (V)
            </h2>
            <span className="text-[10px] bg-emerald-500/20 text-emerald-300 border border-emerald-500/40 px-1.5 py-0.5 rounded font-mono">
              415V RMS
            </span>
          </div>
          <p className="text-[11px] text-slate-400 mb-3">Microgrid voltage sag disturbance simulation (100ms - 200ms)</p>
          
          <div className="flex-1 w-full relative">
            <ResponsiveContainer width="100%" height="100%">
              <LineChart data={data} margin={{ top: 10, right: 10, left: -20, bottom: 0 }}>
                <CartesianGrid strokeDasharray="3 3" stroke="#1a2b4c" vertical={false} />
                <XAxis dataKey="time" tickFormatter={(t) => `${(t * 1000).toFixed(0)}ms`} stroke="#64748b" style={{ fontSize: '10px' }} />
                <YAxis stroke="#10b981" style={{ fontSize: '10px' }} />
                <Tooltip contentStyle={{ backgroundColor: '#060b13', borderColor: '#10b981', borderRadius: '6px', fontSize: '11px' }} />
                <Legend wrapperStyle={{ fontSize: '10px' }} />
                <Line type="monotone" dataKey="gridVoltageA" name="Phase A" stroke="#f43f5e" strokeWidth={1.5} dot={false} isAnimationActive={false} />
                <Line type="monotone" dataKey="gridVoltageB" name="Phase B" stroke="#fbbf24" strokeWidth={1.5} dot={false} isAnimationActive={false} />
                <Line type="monotone" dataKey="gridVoltageC" name="Phase C" stroke="#00f0ff" strokeWidth={1.5} dot={false} isAnimationActive={false} />
              </LineChart>
            </ResponsiveContainer>
          </div>
        </div>

        {/* Card 5: 3-Phase Load Current */}
        <div className="bg-[#0a1122] rounded-xl border border-purple-500/30 p-4 flex flex-col shadow-xl aspect-square">
          <div className="flex items-center justify-between mb-2">
            <h2 className="text-sm font-bold text-purple-400 flex items-center gap-2">
              <Activity className="w-4 h-4 text-purple-400" />
              Non-Linear Load Current (A)
            </h2>
            <span className="text-[10px] bg-purple-500/20 text-purple-300 border border-purple-500/40 px-1.5 py-0.5 rounded font-mono">
              Harmonics
            </span>
          </div>
          <p className="text-[11px] text-slate-400 mb-3">Distorted load current with 5th and 7th harmonic components</p>
          
          <div className="flex-1 w-full relative">
            <ResponsiveContainer width="100%" height="100%">
              <LineChart data={data} margin={{ top: 10, right: 10, left: -20, bottom: 0 }}>
                <CartesianGrid strokeDasharray="3 3" stroke="#1a2b4c" vertical={false} />
                <XAxis dataKey="time" tickFormatter={(t) => `${(t * 1000).toFixed(0)}ms`} stroke="#64748b" style={{ fontSize: '10px' }} />
                <YAxis stroke="#a855f7" style={{ fontSize: '10px' }} />
                <Tooltip contentStyle={{ backgroundColor: '#060b13', borderColor: '#a855f7', borderRadius: '6px', fontSize: '11px' }} />
                <Legend wrapperStyle={{ fontSize: '10px' }} />
                <Line type="monotone" dataKey="loadCurrentA" name="Phase A" stroke="#f43f5e" strokeWidth={1.5} dot={false} isAnimationActive={false} />
                <Line type="monotone" dataKey="loadCurrentB" name="Phase B" stroke="#fbbf24" strokeWidth={1.5} dot={false} isAnimationActive={false} />
                <Line type="monotone" dataKey="loadCurrentC" name="Phase C" stroke="#00f0ff" strokeWidth={1.5} dot={false} isAnimationActive={false} />
              </LineChart>
            </ResponsiveContainer>
          </div>
        </div>


      </div>
    </div>
  );
}
