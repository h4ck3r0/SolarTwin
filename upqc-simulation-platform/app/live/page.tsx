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
import { Activity, Sun, Thermometer, Zap, ActivitySquare, Download } from "lucide-react";
import { Tooltip } from "recharts";

export default function LivePredictionDashboard() {
  const [history, setHistory] = useState<any[]>([]);
  const [current, setCurrent] = useState<any>(null);
  const [historyLimit, setHistoryLimit] = useState<number>(60);

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


  const handleExportCSV = () => {
    if (history.length === 0) return;
    const headers = Object.keys(history[0]);
    const rows = history.map(dp => headers.map(k => dp[k]));
    const csvContent = 'data:text/csv;charset=utf-8,' + [headers.join(','), ...rows.map(e => e.join(','))].join('\n');
    const link = document.createElement('a');
    link.setAttribute('href', encodeURI(csvContent));
    link.setAttribute('download', 'live_history.csv');
    document.body.appendChild(link);
    link.click();
    document.body.removeChild(link);
  };

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
      <div className="flex flex-col bg-white border border-slate-200 rounded-xl overflow-hidden relative shadow-md">
        <div className="absolute inset-0 opacity-10 pointer-events-none" 
             style={{ 
               backgroundImage: 'linear-gradient(rgba(0, 0, 0, 0.05) 1px, transparent 1px), linear-gradient(90deg, rgba(0, 0, 0, 0.05) 1px, transparent 1px)', 
               backgroundSize: '20px 20px' 
             }} 
        />
        
        <div className="flex items-center justify-between p-4 bg-white/50 backdrop-blur-sm z-10 border-b border-slate-200">
          <div className="flex items-center gap-2">
            <Icon className="w-5 h-5" style={{ color }} />
            <h3 className="font-semibold text-slate-700 uppercase tracking-wider text-sm">{title}</h3>
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
            <LineChart data={history.slice(-historyLimit)}>
              <CartesianGrid strokeDasharray="3 3" stroke="#e2e8f0" vertical={false} />
              <XAxis dataKey="Temp" hide />
              <Tooltip contentStyle={{ backgroundColor: "#1e293b", color: "#f8fafc", border: "none", borderRadius: "8px" }} itemStyle={{ color: color }} />
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
    <div className="min-h-full bg-[#F8FAFC] text-slate-900 p-6 font-sans">
      <div className="max-w-7xl mx-auto space-y-6">
        
        {/* Header section */}
        <header className="flex flex-col md:flex-row justify-between items-start md:items-center gap-4 pb-6 border-b border-slate-200">
          <div>
            <h1 className="text-3xl font-bold flex items-center gap-3 tracking-tight">
              <Activity className="w-8 h-8 text-emerald-400" />
              Real-Time Telemetry Dashboard
            </h1>
            <p className="text-slate-500 mt-1">Live System Monitoring \& Load Forecasting • RNSIT College, Bangalore Weather</p>
          </div>
          

          {/* Controls */}
          <div className="flex gap-4 items-center">
            <select 
              value={historyLimit} 
              onChange={(e) => setHistoryLimit(Number(e.target.value))}
              className="bg-white border border-slate-200 text-slate-700 text-sm rounded-md px-3 py-2 shadow-sm focus:outline-none focus:ring-2 focus:ring-sky-500"
            >
              <option value={60}>Last 1 Minute</option>
              <option value={300}>Last 5 Minutes</option>
              <option value={900}>Last 15 Minutes</option>
              <option value={3600}>Last 1 Hour</option>
            </select>
            <button 
              onClick={handleExportCSV}
              className="flex items-center gap-2 bg-emerald-600 hover:bg-emerald-700 text-white px-4 py-2 rounded-md text-sm font-semibold transition-colors shadow-sm"
            >
              <Download className="w-4 h-4" />
              Export History
            </button>
          </div>

          {/* Live Weather Feed */}
          <div className="flex gap-4 bg-white shadow-sm p-3 rounded-lg border border-slate-200">
            <div className="flex flex-col items-center px-4 border-r border-slate-200">
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
