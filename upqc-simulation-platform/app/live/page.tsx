"use client";

import React, { useEffect, useState, useMemo } from "react";
import {
  LineChart,
  Line,
  XAxis,
  YAxis,
  CartesianGrid,
  ResponsiveContainer,
} from "recharts";
import { Activity, Sun, Thermometer, Zap, ActivitySquare } from "lucide-react";

export default function LivePredictionDashboard() {
  const [history, setHistory] = useState<any[]>([]);
  const [current, setCurrent] = useState<any>(null);

  useEffect(() => {
    const fetchLiveData = async () => {
      try {
        const res = await fetch("http://localhost:8000/api/live_data");
        const data = await res.json();
        
        // Multiply THD fractions by 100 to convert to percentages
        const processData = (point: any) => {
          if (!point) return point;
          const newPoint = { ...point };
          for (const key in newPoint) {
            if (key.includes("THD")) newPoint[key] = newPoint[key] * 100;
          }
          return newPoint;
        };

        if (data.history) {
          setHistory(data.history.map(processData));
        }
        if (data.current) {
          setCurrent(processData(data.current));
        }
      } catch (err) {
        console.error("Failed to fetch live data", err);
      }
    };

    const interval = setInterval(fetchLiveData, 1000);
    return () => clearInterval(interval);
  }, []);

  const ChartMonitor = ({
    title,
    dataKey,
    color,
    icon: Icon,
    unit,
  }: {
    title: string;
    dataKey: string;
    color: string;
    icon: any;
    unit: string;
  }) => {
    // Generate tick values to fake the oscilloscope look
    const currentValue = current ? current[dataKey] : 0;
    
    return (
      <div className="flex flex-col bg-slate-900 border border-slate-800 rounded-xl overflow-hidden relative shadow-2xl">
        <div className="absolute inset-0 opacity-10 pointer-events-none" 
             style={{ 
               backgroundImage: 'linear-gradient(rgba(255, 255, 255, 0.1) 1px, transparent 1px), linear-gradient(90deg, rgba(255, 255, 255, 0.1) 1px, transparent 1px)', 
               backgroundSize: '20px 20px' 
             }} 
        />
        
        <div className="flex items-center justify-between p-4 bg-slate-900/50 backdrop-blur-sm z-10 border-b border-slate-800">
          <div className="flex items-center gap-2">
            <Icon className="w-5 h-5" style={{ color }} />
            <h3 className="font-semibold text-slate-300 uppercase tracking-wider text-sm">{title}</h3>
          </div>
          <div className="flex items-baseline gap-1">
            <span className="text-2xl font-mono font-bold" style={{ color }}>
              {currentValue ? currentValue.toFixed(2) : "0.00"}
            </span>
            <span className="text-slate-500 text-sm">{unit}</span>
          </div>
        </div>

        <div className="h-48 w-full p-2 z-10">
          <ResponsiveContainer width="100%" height="100%">
            <LineChart data={history}>
              <CartesianGrid strokeDasharray="3 3" stroke="#334155" vertical={false} />
              <YAxis 
                domain={['dataMin - 10', 'dataMax + 10']} 
                hide 
              />
              <Line
                type="monotone"
                dataKey={dataKey}
                stroke={color}
                strokeWidth={3}
                dot={false}
                isAnimationActive={false} // Disable recharts built-in animation for a raw oscilloscope feel
                style={{
                  filter: `drop-shadow(0px 0px 8px ${color}80)`
                }}
              />
            </LineChart>
          </ResponsiveContainer>
        </div>
      </div>
    );
  };

  return (
    <div className="min-h-full bg-slate-950 text-white p-6 font-sans">
      <div className="max-w-7xl mx-auto space-y-6">
        
        {/* Header section */}
        <header className="flex flex-col md:flex-row justify-between items-start md:items-center gap-4 pb-6 border-b border-slate-800">
          <div>
            <h1 className="text-3xl font-bold flex items-center gap-3 tracking-tight">
              <Activity className="w-8 h-8 text-emerald-400" />
              Real-Time Telemetry Dashboard
            </h1>
            <p className="text-slate-400 mt-1">Live System Monitoring \& Load Forecasting • RNSIT College, Bangalore Weather</p>
          </div>
          
          {/* Live Weather Feed */}
          <div className="flex gap-4 bg-slate-900 p-3 rounded-lg border border-slate-800">
            <div className="flex flex-col items-center px-4 border-r border-slate-700">
              <span className="text-xs text-slate-500 uppercase tracking-widest mb-1">Irradiance</span>
              <div className="flex items-center gap-2">
                <Sun className="w-4 h-4 text-amber-400" />
                <span className="font-mono text-xl font-medium text-amber-400">
                  {current?.Irradiance?.toFixed(0) || "---"}
                </span>
              </div>
            </div>
            <div className="flex flex-col items-center px-4">
              <span className="text-xs text-slate-500 uppercase tracking-widest mb-1">Temperature</span>
              <div className="flex items-center gap-2">
                <Thermometer className="w-4 h-4 text-orange-500" />
                <span className="font-mono text-xl font-medium text-orange-500">
                  {current?.Temp?.toFixed(1) || "--"}°C
                </span>
              </div>
            </div>
          </div>
        </header>

        {/* Primary Monitors */}
        <div className="grid grid-cols-1 md:grid-cols-2 gap-6">
          <ChartMonitor 
            title="Active Power (Pac)" 
            dataKey="Pac" 
            color="#10b981" // Emerald
            icon={Zap}
            unit="W"
          />
          <ChartMonitor 
            title="Microgrid Voltage (Vrms_1)" 
            dataKey="Vrms_1" 
            color="#3b82f6" // Blue
            icon={ActivitySquare}
            unit="V"
          />
        </div>

        {/* Secondary Grid */}
        <div className="grid grid-cols-1 md:grid-cols-3 gap-6">
          <ChartMonitor 
            title="Microgrid Current (Irms_1)" 
            dataKey="Irms_1" 
            color="#f59e0b" // Amber
            icon={Activity}
            unit="A"
          />
          <ChartMonitor 
            title="DC Link Voltage (Vdc)" 
            dataKey="Vdc" 
            color="#8b5cf6" // Purple
            icon={Zap}
            unit="V"
          />
          <ChartMonitor 
            title="Voltage THD (THDv_1)" 
            dataKey="THDv_1" 
            color="#ef4444" // Red
            icon={ActivitySquare}
            unit="%"
          />
        </div>

      </div>
    </div>
  );
}
