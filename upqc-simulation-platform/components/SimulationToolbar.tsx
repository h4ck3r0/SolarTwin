'use client';

import React from 'react';
import { Play, Square, RotateCcw, ZoomIn, ZoomOut, Maximize } from 'lucide-react';
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
    <div className="h-8 border-b border-cyan-950 bg-[#060b13] text-slate-100 flex items-center justify-between px-2.5 select-none font-mono text-[10px]">
      {/* Title & Branding */}
      <div className="flex items-center space-x-2">
        <div className="w-4 h-4 rounded bg-cyan-500/20 border border-cyan-400/60 flex items-center justify-center font-bold text-cyan-400 text-[9px]">
          U
        </div>
        <div className="flex items-center space-x-1.5">
          <h1 className="text-[10.5px] font-bold tracking-wider uppercase text-cyan-400 font-mono">
            UPQC Solar Microgrid Platform
          </h1>
          <span className="text-[8px] bg-amber-500/20 text-amber-300 border border-amber-500/40 px-1 py-0.2 rounded font-bold">
            5× 305W PV (1.525 kW)
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
              ? 'bg-slate-800 text-slate-500 cursor-not-allowed border border-slate-700'
              : 'bg-emerald-600 hover:bg-emerald-500 text-white border border-emerald-400/50 shadow-sm'
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
              ? 'bg-slate-900/60 text-slate-600 border border-slate-800 cursor-not-allowed'
              : 'bg-rose-600 hover:bg-rose-500 text-white border border-rose-400/50 shadow-sm'
          }`}
        >
          <Square className="w-3 h-3 fill-current" />
          <span>Stop</span>
        </button>

        <button
          onClick={onReset}
          className="flex items-center space-x-1 px-2 py-0.5 rounded text-[10px] font-medium bg-slate-900 hover:bg-slate-800 text-slate-300 border border-slate-700 transition-all"
        >
          <RotateCcw className="w-3 h-3" />
          <span>Reset</span>
        </button>

        {/* State & Timer Readout */}
        <div className="flex items-center space-x-2 ml-1.5 border-l border-slate-800 pl-2">
          <span className="px-1.5 py-0.2 rounded text-[8.5px] font-bold uppercase tracking-wider bg-cyan-950/60 border border-cyan-800 text-cyan-400">
            {status}
          </span>

          <div className="flex items-baseline space-x-1 bg-black/80 px-2 py-0.5 rounded border border-slate-800 text-[10px]">
            <span className="text-slate-400 text-[9px]">t =</span>
            <span className="text-cyan-400 font-bold">
              {simulationTime.toFixed(4)} s
            </span>
          </div>
        </div>
      </div>

      {/* Canvas Controls */}
      <div className="flex items-center space-x-1 text-[9px] text-slate-400">
        <span className="uppercase text-[8px]">VIEW</span>
        <button onClick={onZoomOut} className="p-0.5 rounded bg-slate-900 border border-slate-800 hover:text-slate-200">
          <ZoomOut className="w-3 h-3" />
        </button>
        <button onClick={onZoomIn} className="p-0.5 rounded bg-slate-900 border border-slate-800 hover:text-slate-200">
          <ZoomIn className="w-3 h-3" />
        </button>
        <button onClick={onFitView} className="px-1.5 py-0.5 rounded bg-slate-900 border border-slate-800 hover:text-slate-200 font-bold text-[8px]">
          Fit 100%
        </button>
      </div>
    </div>
  );
}
