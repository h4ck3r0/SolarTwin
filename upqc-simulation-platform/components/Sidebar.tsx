"use client";

import React, { useState } from "react";
import Link from "next/link";
import { usePathname } from "next/navigation";
import { 
  Menu, 
  LayoutDashboard, 
  Activity, 
  LineChart, 
  Settings,
  ChevronLeft,
  ChevronRight
} from "lucide-react";

export default function Sidebar() {
  const [isMinimized, setIsMinimized] = useState(false);
  const pathname = usePathname();

  const navItems = [
    { name: "Workspace", href: "/", icon: LayoutDashboard },
    { name: "Live Matrix", href: "/live", icon: Activity },
    { name: "Statistics", href: "/statistics", icon: LineChart },
  ];

  return (
    <aside 
      className={`${
        isMinimized ? "w-16" : "w-64"
      } h-screen bg-[#060b13] border-r border-slate-800 transition-all duration-300 flex flex-col z-50 shadow-2xl relative select-none shrink-0`}
    >
      {/* Header */}
      <div className="h-14 flex items-center justify-between px-4 border-b border-slate-800">
        {!isMinimized && (
          <div className="flex items-center gap-2 overflow-hidden whitespace-nowrap">
            <div className="w-6 h-6 rounded bg-cyan-500/20 border border-cyan-400/60 flex items-center justify-center font-bold text-cyan-400 text-xs shrink-0">
              U
            </div>
            <span className="font-mono text-xs font-bold text-cyan-400 tracking-wider">
              SOLAR TWIN
            </span>
          </div>
        )}
        {isMinimized && (
           <div className="w-full flex justify-center">
             <div className="w-6 h-6 rounded bg-cyan-500/20 border border-cyan-400/60 flex items-center justify-center font-bold text-cyan-400 text-xs shrink-0">
                U
             </div>
           </div>
        )}
      </div>

      {/* Toggle Button */}
      <button 
        onClick={() => setIsMinimized(!isMinimized)}
        className="absolute -right-3 top-16 bg-slate-800 border border-slate-700 text-slate-400 hover:text-white rounded-full p-1 shadow-lg z-50 hover:bg-slate-700 transition-colors"
      >
        {isMinimized ? <ChevronRight size={14} /> : <ChevronLeft size={14} />}
      </button>

      {/* Nav Links */}
      <nav className="flex-1 py-6 px-3 space-y-2 overflow-y-auto overflow-x-hidden">
        {navItems.map((item) => {
          const isActive = pathname === item.href;
          return (
            <Link
              key={item.href}
              href={item.href}
              className={`flex items-center gap-3 px-3 py-2.5 rounded-lg transition-colors group ${
                isActive 
                  ? "bg-cyan-900/40 text-cyan-400 border border-cyan-800/50" 
                  : "text-slate-400 hover:bg-slate-800/50 hover:text-slate-200 border border-transparent"
              }`}
            >
              <item.icon className="w-5 h-5 shrink-0" />
              {!isMinimized && (
                <span className="font-medium text-sm whitespace-nowrap">
                  {item.name}
                </span>
              )}
              {/* Tooltip for minimized state */}
              {isMinimized && (
                <div className="absolute left-16 bg-slate-800 text-white text-xs px-2 py-1 rounded opacity-0 invisible group-hover:opacity-100 group-hover:visible transition-all whitespace-nowrap border border-slate-700">
                  {item.name}
                </div>
              )}
            </Link>
          );
        })}
      </nav>

      {/* Footer / Settings */}
      <div className="p-3 border-t border-slate-800">
        <button className="w-full flex items-center gap-3 px-3 py-2.5 rounded-lg text-slate-400 hover:bg-slate-800/50 hover:text-slate-200 transition-colors border border-transparent group">
          <Settings className="w-5 h-5 shrink-0" />
          {!isMinimized && (
            <span className="font-medium text-sm whitespace-nowrap">
              Settings
            </span>
          )}
          {isMinimized && (
            <div className="absolute left-16 bg-slate-800 text-white text-xs px-2 py-1 rounded opacity-0 invisible group-hover:opacity-100 group-hover:visible transition-all whitespace-nowrap border border-slate-700">
              Settings
            </div>
          )}
        </button>
      </div>
    </aside>
  );
}
