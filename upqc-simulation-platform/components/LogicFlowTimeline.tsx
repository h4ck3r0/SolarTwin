import React from 'react';
import { LogicState } from '../lib/logic-analyzer';
import { ArrowRight, ArrowDown } from 'lucide-react';

export default function LogicFlowTimeline({ logic }: { logic: LogicState }) {
  const solarGen = logic.solar.powerKw;
  const loadDemand = logic.load.powerKw;
  const netKw = solarGen - loadDemand;

  return (
    <div className="bg-white p-6 rounded shadow-sm border border-slate-200">
      <h3 className="font-bold text-slate-800 mb-6 border-b pb-2">Power Flow Decision Tree</h3>
      
      <div className="flex flex-col items-center max-w-2xl mx-auto font-mono text-sm">
        
        {/* Step 1: Generation vs Demand */}
        <div className="flex w-full items-center justify-between mb-4">
          <div className="bg-emerald-50 text-emerald-700 border border-emerald-200 p-3 rounded text-center w-1/3">
            <span className="block font-bold text-xs uppercase mb-1">Solar Gen</span>
            <span className="text-lg">{solarGen.toFixed(1)} kW</span>
          </div>
          
          <div className="flex-1 flex flex-col items-center text-slate-400">
            <span className="text-xs mb-1">Compare</span>
            <div className="h-px bg-slate-300 w-full relative">
              <div className="absolute left-1/2 -top-3 -ml-3 bg-white px-1">
                <span className="text-lg text-slate-500">{solarGen >= loadDemand ? '≥' : '<'}</span>
              </div>
            </div>
          </div>
          
          <div className="bg-purple-50 text-purple-700 border border-purple-200 p-3 rounded text-center w-1/3">
            <span className="block font-bold text-xs uppercase mb-1">Load Demand</span>
            <span className="text-lg">{loadDemand.toFixed(1)} kW</span>
          </div>
        </div>
        
        <ArrowDown className="text-slate-300 my-2" />
        
        {/* Step 2: Net Power */}
        <div className={`p-3 rounded text-center w-1/2 mb-4 border ${netKw > 0 ? 'bg-sky-50 border-sky-200 text-sky-700' : 'bg-amber-50 border-amber-200 text-amber-700'}`}>
          <span className="block font-bold text-xs uppercase mb-1">{netKw > 0 ? 'Excess Power' : 'Deficit Power'}</span>
          <span className="text-lg">{Math.abs(netKw).toFixed(1)} kW</span>
        </div>

        <ArrowDown className="text-slate-300 my-2" />
        
        {/* Step 3: Battery Check */}
        <div className="flex w-full items-center justify-between mb-4">
           <div className="p-3 rounded text-center w-2/5 border border-slate-200 bg-slate-50">
             <span className="block font-bold text-xs uppercase mb-1">Battery Check</span>
             <span>SOC: {logic.battery.soc.toFixed(1)}%</span>
           </div>
           <ArrowRight className="text-slate-300" />
           <div className={`p-3 rounded text-center w-2/5 border ${logic.battery.state !== 'Idle' && logic.battery.state !== 'Disconnected' ? 'bg-sky-50 border-sky-200 text-sky-700' : 'bg-slate-50 border-slate-200 text-slate-500'}`}>
             <span className="block font-bold text-xs uppercase mb-1">Battery Action</span>
             <span>{logic.battery.state}</span>
           </div>
        </div>

        <ArrowDown className="text-slate-300 my-2" />
        
        {/* Step 4: Grid Check */}
        <div className="flex w-full items-center justify-between">
           <div className="p-3 rounded text-center w-2/5 border border-slate-200 bg-slate-50">
             <span className="block font-bold text-xs uppercase mb-1">Grid Status</span>
             <span>Connected</span>
           </div>
           <ArrowRight className="text-slate-300" />
           <div className={`p-3 rounded text-center w-2/5 border ${logic.grid.state !== 'Disconnected' && logic.grid.state !== 'Balanced' ? 'bg-sky-50 border-sky-200 text-sky-700' : 'bg-slate-50 border-slate-200 text-slate-500'}`}>
             <span className="block font-bold text-xs uppercase mb-1">Grid Action</span>
             <span>{logic.grid.state}</span>
           </div>
        </div>

      </div>
    </div>
  );
}
