function allfunc = approxAllcoolingdata(alldata)

allfunc = cell(size(alldata));

for i = 1 : numel(allfunc)
    tempdata = alldata{i};
    M = tempdata;
    M = M / 1000;
    x = M(2, :);
    y = M(1, :);
    [fitresult, ~] = coolingCurvecreateFit(x, y);
    
    y2 = fitresult(x);
    table = linearApproximate(x, y2, 10, 0);
        
      
   
    allfunc{i} = table;
    
end


end