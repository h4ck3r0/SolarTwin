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
    <div className="min-h-screen bg-slate-50/50 p-6">
      <div className="max-w-6xl mx-auto space-y-6">
        
        {/* Header */}
        <div className="flex items-center justify-between">
          <div className="flex items-center gap-4">
            <Link href="/" className="p-2 hover:bg-slate-200 rounded-full transition-colors">
              <ArrowLeft className="w-5 h-5 text-slate-600" />
            </Link>
            <div>
              <h1 className="text-2xl font-black text-slate-800 tracking-tight flex items-center gap-2">
                System Logic Insights
              </h1>
              <p className="text-sm text-slate-500 mt-1">Plain-English breakdown of the electrical power flow.</p>
            </div>
          </div>
        </div>

        {/* State Breakdown Cards */}
        <div className="mt-8">
          <h2 className="text-lg font-bold text-slate-800 mb-4 flex items-center gap-2">
            <Lightbulb className="w-5 h-5 text-amber-500" />
            Current States
          </h2>
          <PlainEnglishCards logic={logic} />
        </div>

        {/* Decision Flow Timeline */}
        <div className="mt-8">
           <LogicFlowTimeline logic={logic} />
        </div>
        
      </div>
    </div>
  );
}
