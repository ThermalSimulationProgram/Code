function alldata = parseAllwarmingdata()
    nstage = 4;
    
    
    allslopes = 1 : 5 : 95;
    alldata = cell(1, numel(allslopes));
    j = 1;
    for slope = allslopes
        temp = cell(1, nstage);
        for i = 1 : nstage
            name = ['stage_', num2str(i), '_slope_', num2str(slope), '.csv'];
            M = csvread(name);
            temp{i} = M;
        end
        alldata{j} = temp;
        j = j + 1;
    end
        
        
   

end