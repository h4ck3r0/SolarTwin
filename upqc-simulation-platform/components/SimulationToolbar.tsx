'use client';

import React from 'react';
import { Play, Square, RotateCcw, ZoomIn, ZoomOut, Maximize, LineChart as ChartIcon, Activity } from 'lucide-react';
import Link from 'next/link';
import { SimulationStatus } from '@/lib/simulation-types';

interface SimulationToolbarProps {
  status: SimulationStatus;
  simulationTime: number;
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
  onRun,
  onStop,
  onReset,
  onZoomIn,
  onZoomOut,
  onFitView,
}: SimulationToolbarProps) {
  return (
    <div className="h-8 border-b border-slate-200 bg-white text-slate-800 flex items-center justify-between px-2.5 select-none font-mono text-[10px]">
      {/* Title & Branding */}
      <div className="flex items-center space-x-2">
        <div className="w-4 h-4 rounded bg-sky-100 border border-sky-300 flex items-center justify-center font-bold text-sky-700 text-[9px]">
          U
        </div>
        <div className="flex items-center space-x-1.5">
          <h1 className="text-[10.5px] font-bold tracking-wider uppercase text-sky-700 font-mono">
            UPQC Solar Microgrid Platform
          </h1>
          <span className="text-[8px] bg-amber-50 text-amber-700 border border-amber-200 px-1 py-0.2 rounded font-bold">
            7S×88P 415W PV (255.6 kW)
          </span>
        </div>
      </div>

      {/* Solver Controls */}
      <div className="flex items-center space-x-1.5">
        <button
          onClick={onRun}
          disabled={status === 'RUNNING'}
          className={`flex items-center space-x-1 px-2.5 py-0.5 rounded text-[10px] font-bold transition-all ${
            status === 'RUNNING'
              ? 'bg-slate-100 text-slate-500 cursor-not-allowed border border-slate-200'
              : 'bg-emerald-600 hover:bg-emerald-500 text-slate-800 border border-emerald-400/50 shadow-sm'
          }`}
        >
          <Play className="w-3 h-3 fill-current" />
          <span>Run</span>
        </button>

        <button
          onClick={onStop}
          disabled={status !== 'RUNNING'}
          className={`flex items-center space-x-1 px-2.5 py-0.5 rounded text-[10px] font-bold transition-all ${
            status !== 'RUNNING'
              ? 'bg-slate-50 text-slate-500 border border-slate-200 cursor-not-allowed'
              : 'bg-rose-600 hover:bg-rose-500 text-slate-800 border border-rose-400/50 shadow-sm'
          }`}
        >
          <Square className="w-3 h-3 fill-current" />
          <span>Stop</span>
        </button>

        <button
          onClick={onReset}
          className="flex items-center space-x-1 px-2 py-0.5 rounded text-[10px] font-medium bg-white hover:bg-slate-50 text-slate-600 border border-slate-300 transition-all"
        >
          <RotateCcw className="w-3 h-3" />
          <span>Reset</span>
        </button>

        <Link
          href="/statistics"
          className="flex items-center space-x-1 px-2 py-0.5 rounded text-[10px] font-medium bg-amber-50 hover:bg-amber-100 text-amber-700 border border-amber-200 transition-all ml-1"
        >
          <ChartIcon className="w-3 h-3" />
          <span>Stats</span>
        </Link>

        <Link
          href="/live"
          className="flex items-center space-x-1 px-2 py-0.5 rounded text-[10px] font-medium bg-emerald-50 hover:bg-emerald-100 text-emerald-700 border border-emerald-200 transition-all ml-1"
        >
          <Activity className="w-3 h-3" />
          <span>Live</span>
        </Link>

        {/* State & Timer Readout */}
        <div className="flex items-center space-x-2 ml-1.5 border-l border-slate-200 pl-2">
          <span className="px-1.5 py-0.2 rounded text-[8.5px] font-bold uppercase tracking-wider bg-sky-50 border border-sky-200 text-sky-700">
            {status}
          </span>

          <div className="flex items-baseline space-x-1 bg-sky-50 px-2 py-0.5 rounded border border-slate-200 text-[10px]">
            <span className="text-slate-500 text-[9px]">t =</span>
            <span className="text-sky-700 font-bold">
              {simulationTime.toFixed(4)} s
            </span>
          </div>
        </div>
      </div>

      {/* Canvas Controls */}
      <div className="flex items-center space-x-1 text-[9px] text-slate-500">
        <span className="uppercase text-[8px]">VIEW</span>
        <button onClick={onZoomOut} className="p-0.5 rounded bg-slate-100 border border-slate-200 hover:text-slate-800">
          <ZoomOut className="w-3 h-3" />
        </button>
        <button onClick={onZoomIn} className="p-0.5 rounded bg-slate-100 border border-slate-200 hover:text-slate-800">
          <ZoomIn className="w-3 h-3" />
        </button>
        <button onClick={onFitView} className="px-1.5 py-0.5 rounded bg-slate-100 border border-slate-200 hover:text-slate-800 font-bold text-[8px]">
          Fit 100%
        </button>
      </div>
    </div>
  );
}
