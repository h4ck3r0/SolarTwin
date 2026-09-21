import React from 'react';
import { LogicState } from '../lib/logic-analyzer';
import { ArrowRight, ArrowDown, Sun, Zap, Battery, BatteryCharging, Network, Activity } from 'lucide-react';

export default function LogicFlowTimeline({ logic }: { logic: LogicState }) {
  const solarGen = logic.solar.powerKw;
  const loadDemand = logic.load.powerKw;
  const netKw = solarGen - loadDemand;
  const isSurplus = netKw > 0;

  return (
    <div className="bg-slate-900 text-slate-100 p-8 rounded-2xl shadow-2xl border border-slate-700/50 overflow-hidden relative">
      {/* Decorative background gradients */}
      <div className="absolute top-0 left-0 w-full h-full overflow-hidden pointer-events-none">
        <div className="absolute top-[-10%] left-[-10%] w-[40%] h-[40%] rounded-full bg-emerald-500/10 blur-[100px]" />
        <div className="absolute bottom-[-10%] right-[-10%] w-[40%] h-[40%] rounded-full bg-sky-500/10 blur-[100px]" />
      </div>

      <h3 className="font-bold text-2xl text-transparent bg-clip-text bg-gradient-to-r from-emerald-400 to-sky-400 mb-8 relative z-10 flex items-center gap-3">
        <Activity className="text-emerald-400" /> 
        Power Flow Decision Engine
      </h3>
      
      <div className="flex flex-col items-center max-w-3xl mx-auto font-sans relative z-10">
        
        {/* Step 1: Generation vs Demand */}
        <div className="flex w-full items-center justify-between mb-8 group">
          {/* Solar Gen Node */}
          <div className="relative flex flex-col items-center p-5 rounded-xl bg-slate-800/80 border border-emerald-500/30 w-[35%] shadow-[0_0_30px_-5px_rgba(16,185,129,0.15)] transition-transform hover:scale-105 backdrop-blur-md">
            <div className="absolute -top-3 bg-slate-900 border border-emerald-500/50 text-emerald-400 p-1.5 rounded-full shadow-[0_0_15px_rgba(16,185,129,0.4)]">
              <Sun size={18} />
            </div>
            <span className="block font-medium text-xs text-slate-400 uppercase tracking-wider mb-2 mt-2">Solar Generation</span>
            <span className="text-3xl font-light text-emerald-400">{solarGen.toFixed(1)}<span className="text-sm text-slate-500 ml-1">kW</span></span>
          </div>
          
          {/* Comparison Badge */}
          <div className="flex-1 flex flex-col items-center justify-center relative px-4">
            <div className="h-px bg-gradient-to-r from-emerald-500/50 via-slate-500 to-amber-500/50 w-full absolute top-1/2 -translate-y-1/2"></div>
            <div className="bg-slate-900 border border-slate-600 px-4 py-2 rounded-full z-10 shadow-xl flex flex-col items-center min-w-[100px]">
              <span className="text-[10px] text-slate-400 uppercase tracking-widest mb-1">Compare</span>
              <span className={`text-xl font-bold ${isSurplus ? 'text-emerald-400' : 'text-amber-400'}`}>
                {isSurplus ? '≥' : '<'}
              </span>
            </div>
          </div>
          
          {/* Load Demand Node */}
          <div className="relative flex flex-col items-center p-5 rounded-xl bg-slate-800/80 border border-amber-500/30 w-[35%] shadow-[0_0_30px_-5px_rgba(245,158,11,0.15)] transition-transform hover:scale-105 backdrop-blur-md">
            <div className="absolute -top-3 bg-slate-900 border border-amber-500/50 text-amber-400 p-1.5 rounded-full shadow-[0_0_15px_rgba(245,158,11,0.4)]">
              <Zap size={18} />
            </div>
            <span className="block font-medium text-xs text-slate-400 uppercase tracking-wider mb-2 mt-2">Load Demand</span>
            <span className="text-3xl font-light text-amber-400">{loadDemand.toFixed(1)}<span className="text-sm text-slate-500 ml-1">kW</span></span>
          </div>
        </div>
        
        <ArrowDown className="text-slate-600 my-2 animate-bounce" size={24} />
        
        {/* Step 2: Net Power */}
        <div className={`relative flex flex-col items-center p-5 rounded-xl border w-[50%] mb-8 transition-all hover:scale-105 backdrop-blur-md shadow-2xl
          ${isSurplus 
            ? 'bg-sky-900/30 border-sky-500/50 shadow-[0_0_40px_-10px_rgba(14,165,233,0.3)]' 
            : 'bg-rose-900/30 border-rose-500/50 shadow-[0_0_40px_-10px_rgba(244,63,94,0.3)]'}`}>
          <div className="absolute -inset-1 rounded-xl bg-gradient-to-r from-transparent via-white/10 to-transparent blur-sm opacity-50 pointer-events-none"></div>
          <span className={`block font-bold text-xs uppercase tracking-widest mb-2 ${isSurplus ? 'text-sky-400' : 'text-rose-400'}`}>
            {isSurplus ? 'Surplus Power' : 'Deficit Power'}
          </span>
          <span className="text-4xl font-light text-white">{Math.abs(netKw).toFixed(1)}<span className="text-lg text-slate-400 ml-1">kW</span></span>
        </div>

        <ArrowDown className="text-slate-600 my-2 animate-bounce" size={24} />
        
        {/* Step 3: Battery Check */}
        <div className="flex w-full items-stretch justify-between mb-8 group h-24">
           <div className="flex flex-col items-center justify-center p-4 rounded-xl border border-slate-700 bg-slate-800/80 w-[40%] backdrop-blur-md">
             <div className="flex items-center gap-2 mb-2 text-slate-300">
               <Battery size={16} />
               <span className="font-bold text-xs uppercase tracking-widest">Battery Status</span>
             </div>
             <div className="w-full bg-slate-900 rounded-full h-2.5 mb-1 overflow-hidden border border-slate-700">
               <div className="bg-gradient-to-r from-emerald-600 to-emerald-400 h-2.5 rounded-full transition-all duration-500" style={{ width: `${logic.battery.soc}%` }}></div>
             </div>
             <span className="text-sm font-medium text-emerald-400">{logic.battery.soc.toFixed(1)}%</span>
           </div>
           
           <div className="flex-1 flex items-center justify-center relative">
             <div className="absolute w-full h-0.5 bg-slate-700"></div>
             <ArrowRight className="text-slate-500 relative z-10 bg-slate-900" size={20} />
           </div>
           
           <div className={`flex flex-col items-center justify-center p-4 rounded-xl border w-[40%] backdrop-blur-md transition-colors
             ${logic.battery.state === 'Charging' ? 'bg-emerald-900/30 border-emerald-500/50 shadow-[0_0_20px_-5px_rgba(16,185,129,0.3)]' : 
               logic.battery.state === 'Discharging' ? 'bg-amber-900/30 border-amber-500/50 shadow-[0_0_20px_-5px_rgba(245,158,11,0.3)]' :
               'bg-slate-800/80 border-slate-700'}`}>
             <div className="flex items-center gap-2 mb-2">
               {logic.battery.state === 'Charging' ? <BatteryCharging size={16} className="text-emerald-400" /> : <Battery size={16} className="text-slate-400" />}
               <span className="font-bold text-xs uppercase tracking-widest text-slate-300">Battery Action</span>
             </div>
             <span className={`text-xl font-medium ${logic.battery.state === 'Charging' ? 'text-emerald-400' : logic.battery.state === 'Discharging' ? 'text-amber-400' : 'text-slate-500'}`}>
               {logic.battery.state}
             </span>
           </div>
        </div>

        <ArrowDown className="text-slate-600 my-2 animate-bounce" size={24} />
        
        {/* Step 4: Grid Check */}
        <div className="flex w-full items-stretch justify-between h-24">
           <div className="flex flex-col items-center justify-center p-4 rounded-xl border border-slate-700 bg-slate-800/80 w-[40%] backdrop-blur-md">
             <div className="flex items-center gap-2 mb-2 text-slate-300">
               <Network size={16} />
               <span className="font-bold text-xs uppercase tracking-widest">Main Grid</span>
             </div>
             <span className="text-sm font-medium text-sky-400 flex items-center gap-2">
               <span className="w-2 h-2 rounded-full bg-sky-500 animate-pulse"></span> Connected
             </span>
           </div>
           
           <div className="flex-1 flex items-center justify-center relative">
             <div className="absolute w-full h-0.5 bg-slate-700"></div>
             <ArrowRight className="text-slate-500 relative z-10 bg-slate-900" size={20} />
           </div>
           
           <div className={`flex flex-col items-center justify-center p-4 rounded-xl border w-[40%] backdrop-blur-md transition-colors
             ${logic.grid.state === 'Exporting' ? 'bg-sky-900/30 border-sky-500/50 shadow-[0_0_20px_-5px_rgba(14,165,233,0.3)]' : 
               logic.grid.state === 'Importing' ? 'bg-indigo-900/30 border-indigo-500/50 shadow-[0_0_20px_-5px_rgba(99,102,241,0.3)]' :
               'bg-slate-800/80 border-slate-700'}`}>
             <div className="flex items-center gap-2 mb-2">
               <Activity size={16} className={logic.grid.state !== 'Balanced' ? 'text-sky-400' : 'text-slate-400'} />
               <span className="font-bold text-xs uppercase tracking-widest text-slate-300">Grid Action</span>
             </div>
             <span className={`text-xl font-medium ${logic.grid.state === 'Exporting' ? 'text-sky-400' : logic.grid.state === 'Importing' ? 'text-indigo-400' : 'text-slate-500'}`}>
               {logic.grid.state}
             </span>
           </div>
        </div>

      </div>
    </div>
  );
}
