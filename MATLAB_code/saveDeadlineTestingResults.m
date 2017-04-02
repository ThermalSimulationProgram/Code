prefix = 'ARM4Test2_deadlinefactor_';

allfactor = 70 : 2 : 94;
for f = allfactor
    
    file = [prefix, num2str(f), '.mat'];
    load(file);
    
    solution = resultData.resultFBPT.solution ;
    csvfile = [prefix, num2str(f*10), '.csv'];
    csvwrite(csvfile,solution);
end