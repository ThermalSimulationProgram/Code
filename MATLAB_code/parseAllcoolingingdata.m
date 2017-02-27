function alldata = parseAllcoolingingdata()
    nstage = 4;
    
    alldata = cell(1, nstage);
    

    
    for i = 1 : nstage
        name = ['stage_', num2str(i), '_cooling_.csv'];
        M = csvread(name);
        alldata{i} = M;
    end

   
       

end