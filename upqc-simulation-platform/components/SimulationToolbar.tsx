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
  
  const getStatusColor = (status: SimulationStatus) => {
    switch (status) {
      case 'RUNNING':
        return 'bg-amber-500 text-black animate-pulse';
      case 'COMPLETED':
        return 'bg-emerald-600 text-white';
      case 'FAILED':
        return 'bg-rose-600 text-white';
      case 'IDLE':
      default:
        return 'bg-slate-700 text-slate-300';
    }
  };

  return (
    <div className="h-12 border-b border-slate-700 bg-slate-900 text-slate-100 flex items-center justify-between px-4 select-none">
      {/* Title & Branding */}
      <div className="flex items-center space-x-3">
        <div className="flex space-x-1">
          <span className="w-2 h-5 bg-blue-600 block rounded-sm"></span>
          <span className="w-1.5 h-5 bg-amber-500 block rounded-sm"></span>
        </div>
        <div>
          <h1 className="text-sm font-semibold tracking-wide uppercase font-mono">
            UPQC Power Quality Simulation
          </h1>
          <span className="text-[10px] text-slate-400 block -mt-1 font-mono">
            V1.1.0-MOCK_SOLVER
          </span>
        </div>
      </div>

      {/* Solver Controls */}
      <div className="flex items-center space-x-1 border-l border-r border-slate-700 px-4">
        <button
          onClick={onRun}
          disabled={status === 'RUNNING'}
          className={`flex items-center space-x-1.5 px-3 py-1 rounded text-xs font-medium transition-all ${
            status === 'RUNNING'
              ? 'bg-slate-800 text-slate-500 cursor-not-allowed'
              : 'bg-emerald-600 hover:bg-emerald-500 text-white shadow-sm'
          }`}
          title="Run Simulation"
        >
          <Play className="w-3.5 h-3.5 fill-current" />
          <span>Run</span>
        </button>

        <button
          onClick={onStop}
          disabled={status !== 'RUNNING'}
          className={`flex items-center space-x-1.5 px-3 py-1 rounded text-xs font-medium transition-all ${
            status !== 'RUNNING'
              ? 'bg-slate-800 text-slate-500 cursor-not-allowed'
              : 'bg-rose-600 hover:bg-rose-500 text-white shadow-sm'
          }`}
          title="Stop Simulation"
        >
          <Square className="w-3.5 h-3.5 fill-current" />
          <span>Stop</span>
        </button>

        <button
          onClick={onReset}
          className="flex items-center space-x-1.5 px-3 py-1 rounded text-xs font-medium bg-slate-800 hover:bg-slate-700 text-slate-300 border border-slate-700 transition-all"
          title="Reset Simulation"
        >
          <RotateCcw className="w-3.5 h-3.5" />
          <span>Reset</span>
        </button>
      </div>

      {/* Info Display (Status and Time) */}
      <div className="flex items-center space-x-6 text-xs font-mono">
        <div className="flex items-center space-x-2">
          <span className="text-slate-400 text-[11px] uppercase">State:</span>
          <span className={`px-2 py-0.5 rounded text-[10px] font-bold uppercase tracking-wider ${getStatusColor(status)}`}>
            {status}
          </span>
        </div>

        <div className="flex items-center space-x-2">
          <span className="text-slate-400 text-[11px] uppercase">Solver Time:</span>
          <span className="text-amber-400 font-bold bg-slate-950 px-2.5 py-0.5 rounded border border-slate-800 w-24 text-right">
            {simulationTime.toFixed(4)} s
          </span>
        </div>
      </div>

      {/* Canvas Navigation Controls */}
      <div className="flex items-center space-x-1 border-l border-slate-700 pl-4">
        <button
          onClick={onZoomIn}
          className="p-1.5 rounded hover:bg-slate-800 text-slate-400 hover:text-slate-200 transition-colors"
          title="Zoom In"
        >
          <ZoomIn className="w-4 h-4" />
        </button>
        <button
          onClick={onZoomOut}
          className="p-1.5 rounded hover:bg-slate-800 text-slate-400 hover:text-slate-200 transition-colors"
          title="Zoom Out"
        >
          <ZoomOut className="w-4 h-4" />
        </button>
        <button
          onClick={onFitView}
          className="p-1.5 rounded hover:bg-slate-800 text-slate-400 hover:text-slate-200 transition-colors"
          title="Fit View"
        >
          <Maximize className="w-4 h-4" />
        </button>
      </div>
    </div>
  );
}
