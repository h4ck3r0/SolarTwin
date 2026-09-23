import React from 'react';
import { LogicState } from '../lib/logic-analyzer';
import { ArrowRight, ArrowDown, Sun, Zap, Battery, BatteryCharging, Network, Activity } from 'lucide-react';

export default function LogicFlowTimeline({ logic }: { logic: LogicState }) {
  const solarGen = logic.solar.powerKw;
  const loadDemand = logic.load.powerKw;
  const netKw = solarGen - loadDemand;
  const isSurplus = netKw > 0;

  return (
    <div className="bg-white text-slate-800 p-8 rounded-2xl shadow-sm border border-slate-200 overflow-hidden relative">
      {/* Decorative background gradients */}
      <div className="absolute top-0 left-0 w-full h-full overflow-hidden pointer-events-none">
        <div className="absolute top-[-10%] left-[-10%] w-[40%] h-[40%] rounded-full bg-emerald-400/5 blur-[100px]" />
        <div className="absolute bottom-[-10%] right-[-10%] w-[40%] h-[40%] rounded-full bg-sky-400/5 blur-[100px]" />
      </div>

      <h3 className="font-bold text-2xl text-slate-800 mb-8 relative z-10 flex items-center gap-3">
        <Activity className="text-emerald-500" /> 
        Power Flow Decision Engine
      </h3>
      
      <div className="flex flex-col items-center max-w-3xl mx-auto font-sans relative z-10">
        
        {/* Step 1: Generation vs Demand */}
        <div className="flex w-full items-center justify-between mb-8 group">
          {/* Solar Gen Node */}
          <div className="relative flex flex-col items-center p-5 rounded-xl bg-slate-50 border border-emerald-200 w-[35%] shadow-sm transition-transform hover:scale-105 backdrop-blur-md">
            <div className="absolute -top-3 bg-white border border-emerald-200 text-emerald-600 p-1.5 rounded-full shadow-sm">
              <Sun size={18} />
            </div>
            <span className="block font-medium text-xs text-slate-500 uppercase tracking-wider mb-2 mt-2">Solar Generation</span>
            <span className="text-3xl font-light text-emerald-600">{solarGen.toFixed(1)}<span className="text-sm text-slate-400 ml-1">kW</span></span>
          </div>
          
          {/* Comparison Badge */}
          <div className="flex-1 flex flex-col items-center justify-center relative px-4">
            <div className="h-px bg-gradient-to-r from-emerald-300 via-slate-300 to-amber-300 w-full absolute top-1/2 -translate-y-1/2"></div>
            <div className="bg-white border border-slate-200 px-4 py-2 rounded-full z-10 shadow-sm flex flex-col items-center min-w-[100px]">
              <span className="text-[10px] text-slate-500 uppercase tracking-widest mb-1">Compare</span>
              <span className={`text-xl font-bold ${isSurplus ? 'text-emerald-500' : 'text-amber-500'}`}>
                {isSurplus ? '≥' : '<'}
              </span>
            </div>
          </div>
          
          {/* Load Demand Node */}
          <div className="relative flex flex-col items-center p-5 rounded-xl bg-slate-50 border border-amber-200 w-[35%] shadow-sm transition-transform hover:scale-105 backdrop-blur-md">
            <div className="absolute -top-3 bg-white border border-amber-200 text-amber-500 p-1.5 rounded-full shadow-sm">
              <Zap size={18} />
            </div>
            <span className="block font-medium text-xs text-slate-500 uppercase tracking-wider mb-2 mt-2">Load Demand</span>
            <span className="text-3xl font-light text-amber-500">{loadDemand.toFixed(1)}<span className="text-sm text-slate-400 ml-1">kW</span></span>
          </div>
        </div>
        
        <ArrowDown className="text-slate-400 my-2 animate-bounce" size={24} />
        
        {/* Step 2: Net Power */}
        <div className={`relative flex flex-col items-center p-5 rounded-xl border w-[50%] mb-8 transition-all hover:scale-105 backdrop-blur-md shadow-md
          ${isSurplus 
            ? 'bg-sky-50 border-sky-200 shadow-sky-100' 
            : 'bg-rose-50 border-rose-200 shadow-rose-100'}`}>
          <div className="absolute -inset-1 rounded-xl bg-gradient-to-r from-transparent via-white/50 to-transparent blur-sm opacity-50 pointer-events-none"></div>
          <span className={`block font-bold text-xs uppercase tracking-widest mb-2 ${isSurplus ? 'text-sky-600' : 'text-rose-600'}`}>
            {isSurplus ? 'Surplus Power' : 'Deficit Power'}
          </span>
          <span className={`text-4xl font-light ${isSurplus ? 'text-sky-700' : 'text-rose-700'}`}>{Math.abs(netKw).toFixed(1)}<span className="text-lg opacity-60 ml-1">kW</span></span>
        </div>

        <ArrowDown className="text-slate-400 my-2 animate-bounce" size={24} />
        
        {/* Step 3: Battery Check */}
        <div className="flex w-full items-stretch justify-between mb-8 group h-24">
           <div className="flex flex-col items-center justify-center p-4 rounded-xl border border-slate-200 bg-slate-50 w-[40%] backdrop-blur-md">
             <div className="flex items-center gap-2 mb-2 text-slate-600">
               <Battery size={16} />
               <span className="font-bold text-xs uppercase tracking-widest">Battery Status</span>
             </div>
             <div className="w-full bg-slate-200 rounded-full h-2.5 mb-1 overflow-hidden border border-slate-300">
               <div className="bg-gradient-to-r from-emerald-500 to-emerald-400 h-2.5 rounded-full transition-all duration-500" style={{ width: `${logic.battery.soc}%` }}></div>
             </div>
             <span className="text-sm font-medium text-emerald-600">{logic.battery.soc.toFixed(1)}%</span>
           </div>
           
           <div className="flex-1 flex items-center justify-center relative">
             <div className="absolute w-full h-0.5 bg-slate-200"></div>
             <ArrowRight className="text-slate-400 relative z-10 bg-white" size={20} />
           </div>
           
           <div className={`flex flex-col items-center justify-center p-4 rounded-xl border w-[40%] backdrop-blur-md transition-colors
             ${logic.battery.state === 'Charging' ? 'bg-emerald-50 border-emerald-200 shadow-sm' : 
               logic.battery.state === 'Discharging' ? 'bg-amber-50 border-amber-200 shadow-sm' :
               'bg-slate-50 border-slate-200'}`}>
             <div className="flex items-center gap-2 mb-2">
               {logic.battery.state === 'Charging' ? <BatteryCharging size={16} className="text-emerald-500" /> : <Battery size={16} className="text-slate-500" />}
               <span className="font-bold text-xs uppercase tracking-widest text-slate-600">Battery Action</span>
             </div>
             <span className={`text-xl font-medium ${logic.battery.state === 'Charging' ? 'text-emerald-600' : logic.battery.state === 'Discharging' ? 'text-amber-600' : 'text-slate-600'}`}>
               {logic.battery.state}
             </span>
           </div>
        </div>

        <ArrowDown className="text-slate-400 my-2 animate-bounce" size={24} />
        
        {/* Step 4: Grid Check */}
        <div className="flex w-full items-stretch justify-between h-24">
           <div className="flex flex-col items-center justify-center p-4 rounded-xl border border-slate-200 bg-slate-50 w-[40%] backdrop-blur-md">
             <div className="flex items-center gap-2 mb-2 text-slate-600">
               <Network size={16} />
               <span className="font-bold text-xs uppercase tracking-widest">Main Grid</span>
             </div>
             <span className={`text-sm font-bold flex items-center gap-2 ${
               logic.grid.state === 'Disconnected' ? 'text-rose-600' :
               logic.grid.state === 'Supplying Load' ? 'text-indigo-600' :
               logic.grid.state === 'Absorbing Excess' ? 'text-sky-600' :
               'text-emerald-600'
             }`}>
               <span className={`w-2 h-2 rounded-full animate-pulse ${
                 logic.grid.state === 'Disconnected' ? 'bg-rose-500' :
                 logic.grid.state === 'Supplying Load' ? 'bg-indigo-500' :
                 logic.grid.state === 'Absorbing Excess' ? 'bg-sky-500' :
                 'bg-emerald-500'
               }`} />
               {logic.grid.state === 'Disconnected' ? 'Offline' : 'Connected'}
             </span>
           </div>
           
           <div className="flex-1 flex items-center justify-center relative">
             <div className="absolute w-full h-0.5 bg-slate-200"></div>
             <ArrowRight className="text-slate-400 relative z-10 bg-white" size={20} />
           </div>
           
           <div className={`flex flex-col items-center justify-center p-4 rounded-xl border w-[40%] backdrop-blur-md transition-colors
             ${logic.grid.state === 'Absorbing Excess' ? 'bg-sky-50 border-sky-200 shadow-sm' : 
               logic.grid.state === 'Supplying Load' ? 'bg-indigo-50 border-indigo-200 shadow-sm' :
               'bg-slate-50 border-slate-200'}`}>
             <div className="flex items-center gap-2 mb-2">
               <Activity size={16} className={logic.grid.state !== 'Balanced' ? 'text-sky-500' : 'text-slate-500'} />
               <span className="font-bold text-xs uppercase tracking-widest text-slate-600">Grid Action</span>
             </div>
             <span className={`text-xl font-medium ${logic.grid.state === 'Absorbing Excess' ? 'text-sky-600' : logic.grid.state === 'Supplying Load' ? 'text-indigo-600' : 'text-slate-600'}`}>
               {logic.grid.state}
             </span>
           </div>
        </div>

      </div>
    </div>
  );
}
