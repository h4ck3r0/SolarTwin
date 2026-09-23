'use client';

import React, { useState, useEffect } from 'react';
import { SimulationDataPoint, SimulationParameters } from '@/lib/simulation-types';
import { analyzeLogic, LogicState } from '@/lib/logic-analyzer';
import PlainEnglishCards from '@/components/PlainEnglishCards';
import LogicFlowTimeline from '@/components/LogicFlowTimeline';
import Link from 'next/link';
import { Lightbulb, Activity, BrainCircuit, BarChart2 } from 'lucide-react';

export default function InsightsPage() {
  const [data, setData] = useState<SimulationDataPoint[]>([]);
  const [params, setParams] = useState<SimulationParameters | null>(null);
  const [logic, setLogic] = useState<LogicState | null>(null);
  const [isLoading, setIsLoading] = useState(true);

  useEffect(() => {
    const stored = localStorage.getItem('simulation_results');
    const storedParams = localStorage.getItem('solar_twin_params');
    const storedTopoIds = localStorage.getItem('simulation_topology_ids');

    let parsedData: SimulationDataPoint[] = [];
    let parsedParams: SimulationParameters | null = null;
    let connectedIds: Set<string> | undefined;

    if (stored) {
      try {
        const parsed = JSON.parse(stored);
        if (Array.isArray(parsed) && parsed.length > 0) {
          parsedData = parsed;
          setData(parsed);
        }
      } catch (e) { console.error(e); }
    }

    if (storedParams) {
      try {
        parsedParams = JSON.parse(storedParams);
        setParams(parsedParams);
      } catch (e) { console.error(e); }
    }

    if (storedTopoIds) {
      try {
        const ids: string[] = JSON.parse(storedTopoIds);
        connectedIds = new Set(ids);
      } catch (e) { /* ignore */ }
    }

    if (parsedData.length > 0 && parsedParams) {
      setLogic(analyzeLogic(parsedData, parsedParams, connectedIds));
    }

    setIsLoading(false);
  }, []);

  /* ─── Loading ─────────────────────────────────────────────────────────── */
  if (isLoading) {
    return (
      <div className="min-h-screen bg-slate-50 flex items-center justify-center">
        <div className="flex flex-col items-center gap-4">
          <BrainCircuit className="w-10 h-10 text-emerald-500 animate-pulse" />
          <span className="text-slate-500 font-mono text-sm tracking-widest uppercase animate-pulse">
            Analyzing Logic…
          </span>
        </div>
      </div>
    );
  }

  /* ─── Empty State ────────────────────────────────────────────────────── */
  if (!logic) {
    return (
      <div className="min-h-screen bg-slate-50 flex items-center justify-center p-8 relative overflow-hidden">
        {/* Background orbs */}
        <div className="absolute top-[-20%] left-[-10%] w-[50%] h-[50%] rounded-full bg-emerald-500/10 blur-[120px] pointer-events-none" />
        <div className="absolute bottom-[-20%] right-[-10%] w-[50%] h-[50%] rounded-full bg-sky-500/10 blur-[120px] pointer-events-none" />

        <div className="text-center max-w-md bg-white/80 border border-slate-200 rounded-2xl p-12 shadow-xl backdrop-blur-md relative z-10">
          <div className="w-16 h-16 rounded-2xl bg-slate-50 border border-slate-200 flex items-center justify-center mx-auto mb-6 shadow-sm">
            <Lightbulb className="w-8 h-8 text-slate-400" />
          </div>
          <h2 className="text-xl font-black text-slate-800 mb-2 tracking-tight">No Simulation Data</h2>
          <p className="text-slate-500 mb-8 text-sm leading-relaxed">
            Run a simulation on the Workspace canvas first to generate the AI Logic Breakdown.
          </p>
          <Link
            href="/"
            className="inline-flex items-center gap-2 px-6 py-3 bg-emerald-600 hover:bg-emerald-500 text-white rounded-xl font-bold text-sm shadow-md shadow-emerald-500/20 transition-all hover:-translate-y-0.5"
          >
            Go to Workspace →
          </Link>
        </div>
      </div>
    );
  }

  /* ─── Main Page ───────────────────────────────────────────────────────── */
  const solarKw = logic.solar.powerKw;
  const loadKw  = logic.load.powerKw;
  const netKw   = solarKw - loadKw;
  const soc     = logic.battery.soc;

  return (
    <div className="min-h-screen bg-slate-50 text-slate-900 selection:bg-emerald-500/30 relative overflow-x-hidden">

      {/* ── Ambient background gradients ──────────────────────────────── */}
      <div className="fixed top-0 left-0 w-full h-full pointer-events-none overflow-hidden -z-0">
        <div className="absolute top-[-15%] left-[-5%] w-[45%] h-[45%] rounded-full bg-emerald-400/10 blur-[140px]" />
        <div className="absolute bottom-[-15%] right-[-5%] w-[45%] h-[45%] rounded-full bg-sky-400/10 blur-[140px]" />
        <div className="absolute top-[40%] left-[50%] w-[30%] h-[30%] rounded-full bg-indigo-400/5 blur-[120px]" />
      </div>

      <div className="relative z-10 max-w-7xl mx-auto px-6 py-8 space-y-10">

        {/* ── Header ─────────────────────────────────────────────────── */}
        <div className="flex items-center justify-between pb-8 border-b border-slate-200/80">
          <div className="flex items-center gap-5">
            <div>
              <div className="flex items-center gap-2 mb-1">
                <BrainCircuit className="w-5 h-5 text-emerald-600" />
                <span className="text-[11px] font-black text-emerald-600 uppercase tracking-[0.2em]">AI Power Analysis</span>
              </div>
              <h1 className="text-3xl font-black text-slate-800 tracking-tight">
                Logic Breakdown
              </h1>
              <p className="text-sm text-slate-500 mt-1.5 font-medium">
                Plain-English breakdown of every power-flow decision made during the simulation.
              </p>
            </div>
          </div>

          {/* Quick stats pill */}
          <Link
            href="/statistics"
            className="hidden md:flex items-center gap-2 px-4 py-2.5 bg-white hover:bg-slate-50 border border-slate-200 rounded-xl text-sm font-bold text-slate-700 hover:text-sky-600 transition-all hover:border-sky-300 shadow-sm"
          >
            <BarChart2 className="w-4 h-4" />
            View Full Stats
          </Link>
        </div>

        {/* ── Snapshot KPI Row ────────────────────────────────────────── */}
        <div className="grid grid-cols-2 md:grid-cols-4 gap-4">
          {[
            {
              label: 'Solar Output',
              value: `${solarKw.toFixed(1)} kW`,
              color: 'emerald',
              sub: logic.solar.state,
            },
            {
              label: 'Load Demand',
              value: `${loadKw.toFixed(1)} kW`,
              color: 'amber',
              sub: 'Active Power',
            },
            {
              label: 'Net Balance',
              value: `${netKw >= 0 ? '+' : ''}${netKw.toFixed(1)} kW`,
              color: netKw >= 0 ? 'sky' : 'rose',
              sub: netKw >= 0 ? 'Surplus → Charging/Export' : 'Deficit → Grid/Battery',
            },
            {
              label: 'Battery SOC',
              value: logic.battery.state === 'Disconnected' ? 'Offline' : `${soc.toFixed(1)}%`,
              color: logic.battery.state === 'Disconnected' ? 'slate' : soc < 20 ? 'rose' : soc < 50 ? 'amber' : 'emerald',
              sub: logic.battery.state,
            },
          ].map((kpi) => (
            <div
              key={kpi.label}
              className={`bg-white border border-slate-200 rounded-2xl p-5 shadow-sm hover:border-slate-300 transition-all`}
            >
              <div className={`text-[10px] font-black uppercase tracking-[0.18em] text-${kpi.color}-600 mb-2`}>{kpi.label}</div>
              <div className={`text-2xl font-black text-${kpi.color}-600 mb-1 tracking-tight`}>{kpi.value}</div>
              <div className="text-[11px] text-slate-500 font-medium">{kpi.sub}</div>
            </div>
          ))}
        </div>

        {/* ── Divider ─────────────────────────────────────────────────── */}
        <div className="flex items-center gap-4">
          <div className="h-px flex-1 bg-gradient-to-r from-transparent via-slate-300 to-transparent" />
          <div className="flex items-center gap-2 px-4 py-1.5 bg-white border border-slate-200 rounded-full shadow-sm">
            <Lightbulb className="w-3.5 h-3.5 text-emerald-500" />
            <span className="text-[10px] font-black uppercase tracking-[0.18em] text-slate-500">AI Diagnostics</span>
          </div>
          <div className="h-px flex-1 bg-gradient-to-r from-transparent via-slate-300 to-transparent" />
        </div>

        {/* ── State Breakdown Cards ────────────────────────────────────── */}
        <PlainEnglishCards logic={logic} />

        {/* ── Divider ─────────────────────────────────────────────────── */}
        <div className="flex items-center gap-4">
          <div className="h-px flex-1 bg-gradient-to-r from-transparent via-slate-300 to-transparent" />
          <div className="flex items-center gap-2 px-4 py-1.5 bg-white border border-slate-200 rounded-full shadow-sm">
            <Activity className="w-3.5 h-3.5 text-sky-500" />
            <span className="text-[10px] font-black uppercase tracking-[0.18em] text-slate-500">Power Flow Engine</span>
          </div>
          <div className="h-px flex-1 bg-gradient-to-r from-transparent via-slate-300 to-transparent" />
        </div>

        {/* ── Power Flow Timeline ──────────────────────────────────────── */}
        <LogicFlowTimeline logic={logic} />

        {/* ── Battery SOC Bar (if connected) ──────────────────────────── */}
        {logic.battery.state !== 'Disconnected' && (
          <div className="bg-white border border-slate-200 rounded-2xl p-6 shadow-sm">
            <div className="flex items-center justify-between mb-4">
              <div>
                <div className="text-[10px] font-black uppercase tracking-[0.18em] text-slate-500 mb-1">Battery State of Charge</div>
                <div className="text-2xl font-black text-emerald-600">{soc.toFixed(2)}%</div>
              </div>
              <div className="text-right">
                <div className="text-[10px] font-black uppercase tracking-[0.18em] text-slate-500 mb-1">Status</div>
                <div className={`text-sm font-bold px-3 py-1 rounded-full ${
                  logic.battery.state === 'Charging' ? 'bg-sky-50 text-sky-600 border border-sky-200' :
                  logic.battery.state === 'Discharging' ? 'bg-amber-50 text-amber-600 border border-amber-200' :
                  'bg-slate-50 text-slate-600 border border-slate-200'
                }`}>
                  {logic.battery.state}
                </div>
              </div>
            </div>
            <div className="w-full bg-slate-100 rounded-full h-3 overflow-hidden border border-slate-200">
              <div
                className={`h-3 rounded-full transition-all duration-700 ${
                  soc > 60 ? 'bg-gradient-to-r from-emerald-500 to-emerald-400' :
                  soc > 30 ? 'bg-gradient-to-r from-amber-500 to-amber-400' :
                  'bg-gradient-to-r from-rose-600 to-rose-500'
                }`}
                style={{ width: `${Math.max(soc, 0)}%` }}
              />
            </div>
            <div className="flex justify-between text-[10px] text-slate-500 font-mono mt-1.5">
              <span>0% Empty</span>
              <span className={`font-bold ${soc > 60 ? 'text-emerald-600' : soc > 30 ? 'text-amber-600' : 'text-rose-600'}`}>
                {soc.toFixed(1)}% →
              </span>
              <span>100% Full</span>
            </div>
            <p className="text-xs text-slate-600 mt-3 leading-relaxed">{logic.battery.description}</p>
          </div>
        )}

        {/* ── Footer ───────────────────────────────────────────────────── */}
        <div className="pb-6 flex items-center justify-between text-[11px] text-slate-500 font-mono border-t border-slate-200/80 pt-6">
          <span>SolarTwin — AI Logic Breakdown</span>
          <Link href="/statistics" className="hover:text-sky-600 transition-colors flex items-center gap-1">
            <BarChart2 className="w-3 h-3" /> View EMT Diagnostics →
          </Link>
        </div>

      </div>
    </div>
  );
}
