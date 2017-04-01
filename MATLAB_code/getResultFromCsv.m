function result = getResultFromCsv(file)

M = csvread(file);

endOfResult = 5e29;
beginOfResult = 1e29;

beginLines = find( M(:,1) > beginOfResult & M(:,1) < endOfResult );
endLines = find(M(:,1) > endOfResult);

numberOfResults = numel(endLines);

if numberOfResults == 0
    result = makeResult(M);
else
    originData = [];
    r = defaultResult(size(M, 2));
    for i = 1 : numberOfResults
        m = M(beginLines(i)+1 : endLines(i)-1, :);
        tmp = makeResult( m );
        originData = [originData, tmp];
        r.TOfCores = r.TOfCores + tmp.TOfCores;
        r.peakT = r.peakT + tmp.peakT;
        r.avrgT = r.avrgT + tmp.avrgT;
        r.peakt = r.peakt + tmp.peakt;
        r.avrgt = r.avrgt + tmp.avrgt;
        r.cpuUsage = r.cpuUsage + tmp.cpuUsage;
    end
    
    r.peakT = r.peakT / numberOfResults;
    r.avrgT = r.avrgT / numberOfResults;
    r.peakt = r.peakt / numberOfResults;
    r.avrgt = r.avrgt / numberOfResults;
    r.cpuUsage = r.cpuUsage / numberOfResults;
    
    result = r;
    result.originData = originData;
    
end






end


function r = makeResult(m)
if size(m, 1) < 4
    error('makeResult: input matrix has less than four rows! incomplete result');
end

scale = 1000;

r.numberOfCores = size(m, 2);
r.TOfCores = m(1, :)/scale;
r.peakT = m(2, 1)/scale;
r.avrgT = m(3, 1)/scale;
r.peakt = m(4, 1);
r.avrgt = m(4, 2);

if size(m, 1) > 4
    r.cpuUsage = m(5, 1) * 100;
else
    r.cpuUsage = 0;
end

r.missDeadlines = m(6:end, :);
end

function r = defaultResult(ncores)

r.numberOfCores = ncores;
r.TOfCores = zeros(1, ncores);
r.peakT = 0;
r.avrgT = 0;
r.peakt = 0;
r.avrgt = 0;
r.cpuUsage = 0;
end