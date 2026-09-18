'use client';

import React from 'react';
import { Play, Square, RotateCcw, ZoomIn, ZoomOut, Maximize, LineChart as ChartIcon, Activity } from 'lucide-react';
import Link from 'next/link';
import { SimulationStatus, SimulationParameters } from '@/lib/simulation-types';

interface SimulationToolbarProps {
  status: SimulationStatus;
  simulationTime: number;
  parameters?: SimulationParameters;   // Phase 5.5: for dynamic PV badge
  onRun: () => void;
  onStop: () => void;
  onReset: () => void;
  onZoomIn: () => void;
  onZoomOut: () => void;
  onFitView: () => void;
}

export default function SimulationToolbar({
  status,
  simulationTime,
  parameters,
  onRun,
  onStop,
  onReset,
  onZoomIn,
  onZoomOut,
  onFitView,
}: SimulationToolbarProps) {
  // Phase 5.5: compute badge string dynamically from current parameters
  const pvBadge = parameters
    ? (() => {
        const s = parameters.solarModulesSeries ?? 7;
        const p = parameters.solarStringsParallel ?? 88;
        const w = parameters.solarPanelWatts ?? 415;
        const kw = ((s * p * w) / 1000).toFixed(1);
        return `${s}S×${p}P ${w}W PV (${kw} kW)`;
      })()
    : '7S×88P 415W PV (255.6 kW)';
  return (
    <div className="h-12 sm:h-14 border-b border-slate-200 bg-white text-slate-800 flex items-center justify-between px-3 sm:px-4 select-none font-mono text-xs sm:text-sm">
      {/* Title & Branding */}
      <div className="flex items-center space-x-2 sm:space-x-3">
        <div className="w-5 h-5 sm:w-6 sm:h-6 rounded bg-sky-100 border border-sky-300 flex items-center justify-center font-bold text-sky-700 text-[10px] sm:text-xs">
          U
        </div>
        <div className="flex items-center space-x-2">
          <h1 className="text-xs sm:text-sm md:text-base font-bold tracking-wider uppercase text-sky-700 font-mono">
            UPQC Solar Microgrid Platform
          </h1>
          <span className="hidden lg:inline-block text-[9px] sm:text-[10px] bg-amber-50 text-amber-700 border border-amber-200 px-1.5 py-0.5 rounded font-bold">
            {pvBadge}
          </span>
        </div>
      </div>

      {/* Solver Controls */}
      <div className="flex items-center space-x-2">
        <button
          onClick={onRun}
          disabled={status === 'RUNNING'}
          className={`flex items-center space-x-1 sm:space-x-1.5 px-3 py-1 sm:px-4 sm:py-1.5 rounded text-xs sm:text-sm font-bold transition-all ${
            status === 'RUNNING'
              ? 'bg-slate-100 text-slate-500 cursor-not-allowed border border-slate-200'
              : 'bg-emerald-600 hover:bg-emerald-500 text-white border border-emerald-400/50 shadow-sm'
          }`}
        >
          <Play className="w-3.5 h-3.5 sm:w-4 sm:h-4 fill-current" />
          <span>Run</span>
        </button>

        <button
          onClick={onStop}
          disabled={status !== 'RUNNING'}
          className={`flex items-center space-x-1 sm:space-x-1.5 px-3 py-1 sm:px-4 sm:py-1.5 rounded text-xs sm:text-sm font-bold transition-all ${
            status !== 'RUNNING'
              ? 'bg-slate-50 text-slate-500 border border-slate-200 cursor-not-allowed'
              : 'bg-rose-600 hover:bg-rose-500 text-slate-800 border border-rose-400/50 shadow-sm'
          }`}
        >
          <Square className="w-3.5 h-3.5 sm:w-4 sm:h-4 fill-current" />
          <span>Stop</span>
        </button>

        <button
          onClick={onReset}
          disabled={status === 'RUNNING'}
          className="flex items-center space-x-1 sm:space-x-1.5 px-3 py-1 sm:px-4 sm:py-1.5 rounded text-xs sm:text-sm font-bold bg-white hover:bg-slate-50 text-slate-600 border border-slate-200 shadow-sm transition-all"
        >
          <RotateCcw className="w-3.5 h-3.5 sm:w-4 sm:h-4" />
          <span className="hidden sm:inline">Reset</span>
        </button>

        <div className="w-px h-6 bg-slate-200 mx-1 sm:mx-2" />

        <Link
          href="/statistics"
          className="flex items-center space-x-1 sm:space-x-1.5 px-3 py-1 sm:px-4 sm:py-1.5 rounded text-xs sm:text-sm font-bold bg-amber-50 hover:bg-amber-100 text-amber-700 border border-amber-200 shadow-sm transition-all"
        >
          <ChartIcon className="w-3.5 h-3.5 sm:w-4 sm:h-4" />
          <span>Stats</span>
        </Link>

        <Link
          href="/live"
          className="flex items-center space-x-1 sm:space-x-1.5 px-3 py-1 sm:px-4 sm:py-1.5 rounded text-xs sm:text-sm font-bold bg-emerald-50 hover:bg-emerald-100 text-emerald-700 border border-emerald-200 shadow-sm transition-all"
        >
          <Activity className="w-3.5 h-3.5 sm:w-4 sm:h-4" />
          <span>Live</span>
        </Link>

        <div className="w-px h-6 bg-slate-200 mx-1 sm:mx-2" />

        {/* Status Indicators */}
        <div className="flex flex-col sm:flex-row items-center sm:space-x-2 mr-2">
          <div className="flex items-center space-x-1.5">
            <div className={`px-2 py-0.5 sm:px-3 sm:py-1 rounded font-bold text-[9px] sm:text-xs ${
              status === 'RUNNING' ? 'bg-emerald-100 text-emerald-700 border border-emerald-200' :
              status === 'FAILED' ? 'bg-rose-100 text-rose-700 border border-rose-200' :
              'bg-slate-100 text-sky-700 border border-slate-200'
            }`}>
              {status}
            </div>
          </div>
          <div className="flex items-center text-[10px] sm:text-sm space-x-1 mt-1 sm:mt-0 font-bold text-sky-700 bg-sky-50 px-2 py-0.5 sm:px-3 sm:py-1 rounded border border-sky-100 min-w-[70px] sm:min-w-[100px] justify-center">
            <span>t =</span>
            <span>{simulationTime.toFixed(4)} s</span>
          </div>
        </div>

        {/* View Controls */}
        <div className="hidden md:flex items-center space-x-1 pl-2 border-l border-slate-200">
          <span className="text-[9px] sm:text-[10px] text-slate-400 font-bold mr-1 uppercase">View</span>
          <button onClick={onZoomIn} className="p-1 sm:p-1.5 hover:bg-slate-100 rounded text-slate-500 hover:text-slate-700 border border-transparent hover:border-slate-200">
            <ZoomIn className="w-3.5 h-3.5 sm:w-4 sm:h-4" />
          </button>
          <button onClick={onZoomOut} className="p-1 sm:p-1.5 hover:bg-slate-100 rounded text-slate-500 hover:text-slate-700 border border-transparent hover:border-slate-200">
            <ZoomOut className="w-3.5 h-3.5 sm:w-4 sm:h-4" />
          </button>
          <button onClick={onFitView} className="px-2 py-1 sm:px-3 sm:py-1 hover:bg-slate-100 rounded text-slate-500 hover:text-slate-700 text-[9px] sm:text-xs font-bold border border-transparent hover:border-slate-200">
            Fit
          </button>
        </div>
      </div>
    </div>
  );
}
