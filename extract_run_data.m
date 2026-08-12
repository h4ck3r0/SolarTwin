function data = extract_run_data(varNames, t_transient, Ts_cycle)
% EXTRACT_RUN_DATA  Pull logged timeseries from base workspace, resample to
% a fixed per-cycle grid, and discard the startup transient.
%
%   data = extract_run_data(varNames, t_transient, Ts_cycle)
%
%   varNames    - cell array of base-workspace timeseries variable names
%   t_transient - seconds to discard at the start of the run
%   Ts_cycle    - sampling interval in seconds (e.g. 1/60 for 60Hz)

    nSamples = zeros(size(varNames));
    for i = 1:numel(varNames)
        if ~evalin('base', sprintf('exist(''%s'',''var'')', varNames{i}))
            error('Variable "%s" not found in base workspace after sim.', varNames{i});
        end
        ts = evalin('base', varNames{i});
        nSamples(i) = numel(ts.Time);
    end

    if any(nSamples < 1)
        bad = varNames(nSamples < 1);
        error('Zero-sample signals: %s', strjoin(bad, ', '));
    end

    multiIdx = find(nSamples >= 2);
    tStarts = arrayfun(@(i) min(evalin('base', varNames{i}).Time), multiIdx);
    tEnds   = arrayfun(@(i) max(evalin('base', varNames{i}).Time), multiIdx);
    tStart  = max(tStarts);
    tEnd    = min(tEnds);

    if tEnd <= tStart + t_transient
        error('Not enough simulated time after transient cutoff.');
    end

    tGrid = (tStart + t_transient) : Ts_cycle : tEnd;
    tGrid = tGrid(:);

    data = table();
    data.Time = tGrid;

    for i = 1:numel(varNames)
        ts = evalin('base', varNames{i});
        t  = ts.Time;
        y  = ts.Data;
        sz = size(y);
        y = reshape(y, sz(1), []);

        if size(y,1) == 1
            vals = repmat(y, numel(tGrid), 1);
        else
            [t, uidx] = unique(t, 'last');
            y = y(uidx, :);
            vals = interp1(t, y, tGrid, 'previous', 'extrap');
        end

        if size(vals,2) == 1
            data.(varNames{i}) = vals;
        else
            for c = 1:size(vals,2)
                data.(sprintf('%s_%d', varNames{i}, c)) = vals(:,c);
            end
        end
    end

    data.Pdc = data.Vpv .* data.Ipv;
end
