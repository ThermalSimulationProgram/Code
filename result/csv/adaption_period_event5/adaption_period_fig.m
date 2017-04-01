
prefix = 'adaption_period';
postfix = '_result.csv';

periods = 200 : 30 : 620;

aptmResults = [];
bwsResults  = [];
pbooResults = [];

kernels = {'aptm', 'bws', 'pboo'};
control = [1, 1, 0];
for p = periods
    
    index = 1;
    for k = kernels
        if control(index)
            file = [prefix, num2str(p), '_', k{1}, postfix];
            r = getResultFromCsv(file);
            switch index
                case 1
                    aptmResults = [aptmResults, r];
                case 2
                    bwsResults  = [bwsResults, r];
                case 3
                    pbooResults = [pbooResults, r];
                otherwise
            end
        end
        
        
        index = index + 1;
        
    end 
end


numberSamples = numel(aptmResults);
avrgT_aptm = zeros(1, numberSamples);
avrgT_bws  = avrgT_aptm;

for i = 1 : numberSamples
    avrgT_aptm(i) = aptmResults(i).avrgT;
    avrgT_bws(i) = bwsResults(i).avrgT;
end

plot(periods, avrgT_aptm, 'b', periods, avrgT_bws, 'r');




