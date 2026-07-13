'use client';

import React, { useEffect, useMemo, useRef } from 'react';
import ReactFlow, {
  Background,
  Controls,
  MiniMap,
  NodeTypes,
  useNodesState,
  useEdgesState,
  Handle,
  Position,
  NodeProps,
  useReactFlow,
  Panel,
} from 'reactflow';
import 'reactflow/dist/style.css';
import { initialNodes, initialEdges, ElectricalNodeData } from '@/lib/model-definition';
import { Cpu, Eye, AlertCircle } from 'lucide-react';

// Custom Node component representing Simulink/Electrical blocks
const CustomElectricalNode = ({ id, data, selected }: NodeProps<ElectricalNodeData>) => {
  // Select matching symbol based on type
  const renderSymbol = () => {
    switch (data.type) {
      case 'source':
        return (
          <svg className="w-12 h-12 text-slate-700" viewBox="0 0 60 60" fill="none" stroke="currentColor" strokeWidth="1.5">
            <circle cx="30" cy="30" r="20" />
            <path d="M20,30 C25,20 25,40 30,30 C35,20 35,40 40,30" />
            <line x1="10" y1="30" x2="2" y2="30" />
            <line x1="50" y1="30" x2="58" y2="30" />
          </svg>
        );
      case 'measurement':
        return (
          <svg className="w-12 h-12 text-blue-600" viewBox="0 0 60 60" fill="none" stroke="currentColor" strokeWidth="1.5">
            <rect x="15" y="15" width="30" height="30" rx="3" />
            <line x1="10" y1="30" x2="15" y2="30" />
            <line x1="45" y1="30" x2="50" y2="30" />
            <circle cx="30" cy="30" r="6" fill="#eff6ff" />
            <path d="M27,33 L30,27 L33,33" />
          </svg>
        );
      case 'transformer':
        return (
          <svg className="w-12 h-12 text-slate-700" viewBox="0 0 60 60" fill="none" stroke="currentColor" strokeWidth="1.5">
            <circle cx="22" cy="30" r="12" />
            <circle cx="38" cy="30" r="12" />
            <line x1="2" y1="30" x2="10" y2="30" />
            <line x1="50" y1="30" x2="58" y2="30" />
          </svg>
        );
      case 'inverter':
        return (
          <svg className="w-12 h-12 text-amber-600" viewBox="0 0 60 60" fill="none" stroke="currentColor" strokeWidth="1.5">
            <rect x="12" y="12" width="36" height="36" rx="4" />
            <line x1="12" y1="48" x2="48" y2="12" />
            <path d="M22,22 L26,18 M38,38 L34,42" strokeWidth="1" />
            <text x="18" y="42" fontSize="10" fill="currentColor" stroke="none" fontWeight="bold">S</text>
            <text x="36" y="24" fontSize="10" fill="currentColor" stroke="none" fontWeight="bold">D</text>
          </svg>
        );
      case 'control':
        return (
          <div className="w-12 h-12 rounded border border-blue-400 bg-blue-50/50 flex flex-col items-center justify-center text-blue-700">
            <Cpu className="w-6 h-6" strokeWidth={1.5} />
            <span className="text-[8px] font-bold mt-0.5">PI</span>
          </div>
        );
      case 'microgrid':
        return (
          <svg className="w-12 h-12 text-emerald-600" viewBox="0 0 60 60" fill="none" stroke="currentColor" strokeWidth="1.5">
            <rect x="10" y="10" width="40" height="40" rx="4" />
            {/* Solar panel grids */}
            <line x1="10" y1="23" x2="50" y2="23" />
            <line x1="10" y1="36" x2="50" y2="36" />
            <line x1="23" y1="10" x2="23" y2="50" />
            <line x1="36" y1="10" x2="36" y2="50" />
            {/* Battery symbol */}
            <rect x="25" y="27" width="10" height="15" fill="#ecfdf5" />
            <line x1="28" y1="24" x2="32" y2="24" strokeWidth="3" />
          </svg>
        );
      case 'load':
        return (
          <svg className="w-12 h-12 text-slate-600" viewBox="0 0 60 60" fill="none" stroke="currentColor" strokeWidth="1.5">
            <line x1="10" y1="30" x2="20" y2="30" />
            {/* Resistor zig zag */}
            <path d="M20,30 L23,20 L27,40 L31,20 L35,40 L39,20 L43,40 L47,30" />
            <line x1="47" y1="30" x2="50" y2="30" />
            {/* Inductor coils */}
            <path d="M20,30 C20,35 25,35 25,30 C25,35 30,35 30,30 C30,35 35,35 35,30 C35,35 40,35 40,30 C40,35 45,35 45,30" stroke="#475569" strokeWidth="1" />
          </svg>
        );
      case 'scope':
        return (
          <div className="w-12 h-12 rounded border border-emerald-500 bg-slate-900 flex flex-col items-center justify-center text-emerald-400">
            <Eye className="w-6 h-6 animate-pulse" />
            <span className="text-[7px] tracking-tighter uppercase mt-0.5">Scope</span>
          </div>
        );
      default:
        return <AlertCircle className="w-8 h-8 text-slate-400" />;
    }
  };

  const getBorderColor = () => {
    if (selected) return 'border-blue-600 ring-2 ring-blue-100 shadow-md';
    if (data.type === 'control') return 'border-blue-300 bg-sky-50/70 hover:border-blue-400';
    if (data.type === 'microgrid') return 'border-emerald-300 bg-emerald-50/70 hover:border-emerald-400';
    if (data.type === 'scope') return 'border-emerald-500 bg-slate-950 text-white';
    return 'border-slate-300 bg-slate-50/90 hover:border-slate-400';
  };

  return (
    <div className={`p-2.5 rounded shadow-sm border transition-all text-slate-800 ${getBorderColor()} w-48`}>
      {/* Handles for electrical flow / connections */}
      <Handle
        type="target"
        position={Position.Left}
        id="left-in"
        style={{ background: '#3b82f6', borderRadius: '1px', width: '6px', height: '6px', left: '-4px' }}
      />
      <Handle
        type="source"
        position={Position.Right}
        id="right-out"
        style={{ background: '#3b82f6', borderRadius: '1px', width: '6px', height: '6px', right: '-4px' }}
      />
      <Handle
        type="target"
        position={Position.Top}
        id="top-in"
        style={{ background: '#f59e0b', borderRadius: '1px', width: '6px', height: '6px', top: '-4px' }}
      />
      <Handle
        type="source"
        position={Position.Bottom}
        id="bottom-out"
        style={{ background: '#f59e0b', borderRadius: '1px', width: '6px', height: '6px', bottom: '-4px' }}
      />

      <div className="flex items-center space-x-3">
        <div className="flex-shrink-0">{renderSymbol()}</div>
        <div className="overflow-hidden">
          <div className="text-[11px] font-bold font-mono text-slate-900 truncate leading-tight">
            {data.label}
          </div>
          <div className="text-[9px] text-slate-500 font-sans truncate leading-none mt-0.5">
            {data.details}
          </div>
          {data.parameters && (
            <div className="text-[8px] text-slate-400 font-mono mt-1 flex flex-wrap gap-x-1.5 leading-none">
              {Object.entries(data.parameters).slice(0, 2).map(([k, v]) => (
                <span key={k} className="bg-slate-100 px-0.5 rounded border border-slate-200">
                  {k.split(' ')[0]}: {v}
                </span>
              ))}
            </div>
          )}
        </div>
      </div>
    </div>
  );
};

interface SimulationCanvasProps {
  isSimulating: boolean;
  selectedNodeId: string | null;
  onSelectNode: (nodeId: string | null) => void;
  // Expose viewport handlers to allow the parent (Toolbar) to control React Flow
  setZoomInRef: React.MutableRefObject<(() => void) | null>;
  setZoomOutRef: React.MutableRefObject<(() => void) | null>;
  setFitViewRef: React.MutableRefObject<(() => void) | null>;
}

export default function SimulationCanvas({
  isSimulating,
  selectedNodeId,
  onSelectNode,
  setZoomInRef,
  setZoomOutRef,
  setFitViewRef,
}: SimulationCanvasProps) {
  const [nodes, setNodes, onNodesChange] = useNodesState(initialNodes);
  const [edges, setEdges, onEdgesChange] = useEdgesState(initialEdges);
  const { zoomIn, zoomOut, fitView, setCenter } = useReactFlow();

  // Keep a reference of nodes to avoid stale closure issues in useEffect without listing nodes as dependency
  const nodesRef = useRef(nodes);
  nodesRef.current = nodes;

  // Attach controls to parent refs for the toolbar
  useEffect(() => {
    setZoomInRef.current = () => zoomIn({ duration: 300 });
    setZoomOutRef.current = () => zoomOut({ duration: 300 });
    setFitViewRef.current = () => fitView({ duration: 300, padding: 0.1 });
  }, [zoomIn, zoomOut, fitView, setZoomInRef, setZoomOutRef, setFitViewRef]);

  // Sync selectedNodeId (e.g. from Explorer) with React Flow selection state and center viewport
  useEffect(() => {
    let changed = false;
    setNodes((nds) => {
      const updated = nds.map((node) => {
        const shouldBeSelected = node.id === selectedNodeId;
        if (node.selected !== shouldBeSelected) {
          changed = true;
          return { ...node, selected: shouldBeSelected };
        }
        return node;
      });
      return changed ? updated : nds;
    });

    if (selectedNodeId) {
      const selectedNode = nodesRef.current.find((n) => n.id === selectedNodeId);
      if (selectedNode) {
        // Node dimensions are w-48 (192px) and h-16 (~64px). Center point is offset.
        const centerX = selectedNode.position.x + 96;
        const centerY = selectedNode.position.y + 32;
        setCenter(centerX, centerY, { zoom: 1.1, duration: 400 });
      }
    }
  }, [selectedNodeId, setCenter]);

  // Animate edges when simulation is running
  useEffect(() => {
    setEdges((eds) =>
      eds.map((edge) => {
        const isControlEdge = edge.style?.stroke === '#2563eb' || edge.style?.stroke === '#16a34a';
        const isDcEdge = edge.style?.stroke === '#ea580c';
        
        return {
          ...edge,
          animated: isSimulating,
          style: {
            ...edge.style,
            // If running, highlight power paths in yellow, control in bright blue, DC in orange
            stroke: isSimulating
              ? isControlEdge
                ? '#3b82f6' // bright blue
                : isDcEdge
                ? '#f97316' // bright orange
                : '#eab308' // bright yellow for AC power lines
              : edge.id.includes('sctrl') || edge.id.includes('shctrl') || edge.id.includes('dc-dcctrl') || edge.id.includes('scope')
              ? '#2563eb' // default blue
              : isDcEdge
              ? '#ea580c' // default orange
              : edge.id.includes('scope')
              ? '#16a34a' // default scope green
              : '#0f172a', // default dark gray power
            strokeWidth: isSimulating ? 2.5 : 2,
          },
        };
      })
    );
  }, [isSimulating, setEdges]);

  // Memoize custom node types
  const nodeTypes = useMemo<NodeTypes>(
    () => ({
      electrical: CustomElectricalNode,
      control: CustomElectricalNode,
      microgrid: CustomElectricalNode,
      scope: CustomElectricalNode,
    }),
    []
  );

  return (
    <div className="flex-1 h-full bg-slate-50 relative overflow-hidden select-none">
      <ReactFlow
        nodes={nodes}
        edges={edges}
        onNodesChange={onNodesChange}
        onEdgesChange={onEdgesChange}
        onNodeClick={(event, node) => onSelectNode(node.id)}
        onPaneClick={() => onSelectNode(null)}
        nodeTypes={nodeTypes}
        fitView
        fitViewOptions={{ padding: 0.1 }}
        minZoom={0.2}
        maxZoom={2}
        selectNodesOnDrag={false}
      >
        <Background color="#cbd5e1" gap={16} size={1} />
        
        {/* Custom canvas HUD overlay panel */}
        <Panel position="top-left" className="m-3 p-2 bg-slate-900/95 text-slate-100 rounded shadow-md border border-slate-800 text-[10px] font-mono leading-relaxed space-y-0.5 max-w-[200px]">
          <div className="font-bold text-slate-300 uppercase tracking-wide border-b border-slate-700 pb-0.5 mb-1 flex items-center justify-between">
            <span>UPQC Canvas HUD</span>
            <span className="w-1.5 h-1.5 rounded-full bg-emerald-500 animate-ping"></span>
          </div>
          <div>Grid Base: <span className="text-amber-400">415 V L-L</span></div>
          <div>Bus Freq: <span className="text-amber-400">50 Hz</span></div>
          <div>Compensation: <span className="text-emerald-400 font-bold">Series + Shunt APF</span></div>
        </Panel>

        <Controls showInteractive={false} className="bg-slate-900 border border-slate-800 rounded shadow-md text-slate-100 fill-current" />
        <MiniMap
          style={{ height: 100, width: 150 }}
          nodeStrokeColor={(n) => {
            if (n.type === 'control') return '#2563eb';
            if (n.type === 'microgrid') return '#10b981';
            return '#64748b';
          }}
          nodeColor={(n) => {
            if (n.type === 'control') return '#eff6ff';
            if (n.type === 'microgrid') return '#ecfdf5';
            return '#f8fafc';
          }}
          maskColor="rgba(241, 245, 249, 0.4)"
        />
      </ReactFlow>
    </div>
  );
}
