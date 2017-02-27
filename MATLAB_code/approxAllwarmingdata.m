function allfunc = approxAllwarmingdata(alldata)

allfunc = cell(size(alldata));

for i = 1 : numel(allfunc)
    tempdata = alldata{i};
    tempfunc = cell(size(tempdata));
    for j = 1 : numel(tempdata)
        M = tempdata{j};
        M = M / 1000;
        x = M(2, :);
        y = M(1, :);
        [fitresult, ~] = warmingcreateFit1(x, y);
        
        y2 = fitresult(x);
        table = linearApproximate(x, y2, 10, 0);
        
        tempfunc{j} = table;
        
    end
    allfunc{i} = tempfunc;
    
end


end