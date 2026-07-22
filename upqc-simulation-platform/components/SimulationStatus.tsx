'use client';

import React, { useState, useEffect } from 'react';
import { Settings, Play, CheckCircle2, AlertCircle } from 'lucide-react';
import { SimulationStatus } from '@/lib/simulation-types';

interface SimulationStatusProps {
  status: SimulationStatus;
}

export default function SimulationStatusWidget({ status }: SimulationStatusProps) {
  const [progress, setProgress] = useState(0);
  const [stage, setStage] = useState('');

  // Simulate solver stages when running
  useEffect(() => {
    if (status !== 'RUNNING') {
      setProgress(0);
      setStage('');
      return;
    }

    setProgress(5);
    setStage('Initializing MATLAB Runtime environment...');

    const timer1 = setTimeout(() => {
      setProgress(25);
      setStage('Loading Simulink diagram (upqc5_7_1_PI_20a_fin_1_1.slx)...');
    }, 500);

    const timer2 = setTimeout(() => {
      setProgress(55);
      setStage('Running solver (Ode45 discrete) with 200 grid sample steps...');
    }, 1200);

    const timer3 = setTimeout(() => {
      setProgress(85);
      setStage('Compiling scope matrices and generating outputs...');
    }, 2000);

    const timer4 = setTimeout(() => {
      setProgress(100);
      setStage('Simulation completed successfully.');
    }, 2450);

    return () => {
      clearTimeout(timer1);
      clearTimeout(timer2);
      clearTimeout(timer3);
      clearTimeout(timer4);
    };
  }, [status]);

  if (status === 'IDLE') return null;

  return (
    <div className="fixed bottom-12 right-[336px] z-40 bg-slate-900/95 backdrop-blur-sm border border-slate-800 rounded shadow-2xl w-80 overflow-hidden font-mono text-xs select-none">
      {/* Header */}
      <div className="h-8 bg-slate-950 px-3 flex items-center justify-between border-b border-slate-850">
        <span className="font-bold tracking-wider text-[10px] text-slate-400 uppercase flex items-center space-x-1.5">
          <Settings className={`w-3.5 h-3.5 ${status === 'RUNNING' ? 'animate-spin' : ''}`} />
          <span>Solver Output Bus</span>
        </span>
        <span className="text-[9px] text-slate-500">MOCK_SOLVER</span>
      </div>

      {/* Content */}
      <div className="p-3.5 space-y-3">
        {status === 'RUNNING' && (
          <div className="space-y-2">
            <div className="flex items-center justify-between text-[11px] font-bold text-amber-500">
              <span className="animate-pulse">Simulation Running...</span>
              <span>{progress}%</span>
            </div>
            
            {/* Progress bar container */}
            <div className="w-full bg-slate-950 rounded-full h-1.5 overflow-hidden border border-slate-850">
              <div
                className="bg-amber-500 h-full rounded-full transition-all duration-300 ease-out"
                style={{ width: `${progress}%` }}
              ></div>
            </div>
            
            <div className="text-[10px] text-slate-400 leading-normal line-clamp-2 h-7">
              {stage}
            </div>
          </div>
        )}

        {status === 'COMPLETED' && (
          <div className="flex items-start space-x-2.5 bg-emerald-950/40 border border-emerald-900/60 p-2.5 rounded">
            <CheckCircle2 className="w-4 h-4 text-emerald-400 flex-shrink-0 mt-0.5" />
            <div className="space-y-0.5">
              <div className="font-bold text-emerald-400 text-[11px]">Simulation Completed</div>
              <div className="text-[9px] text-slate-400">
                Scope bus updated. Select tabs below to plot waveforms.
              </div>
            </div>
          </div>
        )}

        {status === 'FAILED' && (
          <div className="flex items-start space-x-2.5 bg-rose-950/40 border border-rose-900/60 p-2.5 rounded">
            <AlertCircle className="w-4 h-4 text-rose-400 flex-shrink-0 mt-0.5" />
            <div className="space-y-0.5">
              <div className="font-bold text-rose-400 text-[11px]">Solver Error</div>
              <div className="text-[9px] text-slate-400">
                A mathematical singularity occurred. Check parameters.
              </div>
            </div>
          </div>
        )}
      </div>
    </div>
  );
}
