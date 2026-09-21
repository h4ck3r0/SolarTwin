'use client';

import React, { useState, useEffect } from 'react';
import { SimulationDataPoint, SimulationParameters } from '@/lib/simulation-types';
import { analyzeLogic, LogicState } from '@/lib/logic-analyzer';
import PlainEnglishCards from '@/components/PlainEnglishCards';
import LogicFlowTimeline from '@/components/LogicFlowTimeline';
import Link from 'next/link';
import { ArrowLeft, Lightbulb } from 'lucide-react';

export default function InsightsPage() {
  const [data, setData] = useState<SimulationDataPoint[]>([]);
  const [params, setParams] = useState<SimulationParameters | null>(null);
  const [logic, setLogic] = useState<LogicState | null>(null);
  const [isLoading, setIsLoading] = useState(true);

  useEffect(() => {
    const stored = localStorage.getItem('simulation_results');
    const storedParams = localStorage.getItem('solar_twin_params');
    
    let parsedData: SimulationDataPoint[] = [];
    let parsedParams: SimulationParameters | null = null;
    
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
    
    if (parsedData.length > 0 && parsedParams) {
      setLogic(analyzeLogic(parsedData, parsedParams));
    }
    
    setIsLoading(false);
  }, []);

  if (isLoading) {
    return <div className="p-8 text-center text-slate-500">Analyzing logic...</div>;
  }

  if (!logic) {
    return (
      <div className="p-8 max-w-xl mx-auto text-center mt-12 bg-white rounded shadow-sm border border-slate-200">
        <Lightbulb className="w-12 h-12 text-slate-300 mx-auto mb-4" />
        <h2 className="text-xl font-bold text-slate-800 mb-2">No Simulation Data</h2>
        <p className="text-slate-500 mb-6">Run a simulation on the Workspace canvas to see the logic breakdown.</p>
        <Link href="/" className="px-4 py-2 bg-sky-500 text-white rounded font-bold text-sm shadow-sm hover:bg-sky-600 transition-colors">
          Go to Workspace
        </Link>
      </div>
    );
  }

  return (
    <div className="min-h-screen bg-slate-950 p-6 selection:bg-emerald-500/30">
      <div className="max-w-7xl mx-auto space-y-8">
        
        {/* Header */}
        <div className="flex items-center justify-between pb-6 border-b border-slate-800/50">
          <div className="flex items-center gap-6">
            <Link href="/" className="p-3 bg-slate-900 hover:bg-slate-800 border border-slate-700/50 rounded-xl transition-all duration-300 hover:shadow-[0_0_20px_-5px_rgba(16,185,129,0.3)] group">
              <ArrowLeft className="w-5 h-5 text-slate-400 group-hover:text-emerald-400 transition-colors" />
            </Link>
            <div>
              <h1 className="text-3xl font-black text-transparent bg-clip-text bg-gradient-to-r from-emerald-400 via-sky-400 to-indigo-400 tracking-tight flex items-center gap-3">
                System Logic Insights
              </h1>
              <p className="text-sm text-slate-400 mt-2 font-medium tracking-wide">Plain-English breakdown of the electrical power flow and AI decisions.</p>
            </div>
          </div>
        </div>

        {/* State Breakdown Cards */}
        <div className="mt-8 relative z-10">
          <h2 className="text-xl font-bold text-slate-200 mb-6 flex items-center gap-3 tracking-wide">
            <Lightbulb className="w-6 h-6 text-emerald-400" />
            Current AI Diagnostics
          </h2>
          <PlainEnglishCards logic={logic} />
        </div>

        {/* Decision Flow Timeline */}
        <div className="mt-12 mb-12">
           <LogicFlowTimeline logic={logic} />
        </div>
        
      </div>
    </div>
  );
}
