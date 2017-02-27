function [] = saveWarmingData(prefix, allfunc)


slopedata = allfunc;
nstages = numel(slopedata{1});
nslopes = numel(slopedata);

slopestep = 5;

% 
% toffs = 2:200;
%         temps = r(toffs);
%         temps = temps(:)';
%         tempat0 = r(1);
%         slopes = temps - [tempat0, temps(1:end-1)];
        
numtoffs = 20;

for i = 1 : nstages
    A = [];
    D = [];
    for j = 1 : nslopes
        B = [];
        r = slopedata{j}{i};
        toffs = r(:,1)';
        
%         numseg = numel(toffs);
%         if (numseg > numtoffs)
%             error('too small numtoffs');
%         end
%         if (numseg < numtoffs)
%             pointnumeachseg = round(numtoffs/numseg);
%             
%             pointNum = zeros(1, numseg);
%             beginPoint = 2;
%             
%             newtoffs = [];
%             for k = 1 : numseg
%                 if (k == numseg)
%                     pointNum(end) = numtoffs - sum(pointNum(1:end-1));
%                 else
%                     pointNum(k) = pointnumeachseg;
%                 end
%                 subtoffs = linspace(beginPoint, toffs(k), pointNum(k)+1);  
%                 newtoffs = [newtoffs, subtoffs(2:end)];
%                 beginPoint = toffs(k);
%             end
%         end
%         
%         if (numel(newtoffs) ~=numtoffs)
%             error('sdsf');
%         end
%         
%         
%         
%         temps = functemplate(newtoffs, r);
%         temps = temps(:)';
%         tempat0 = functemplate(2, r);
%         slopes = (temps - [tempat0, temps(1:end-1)]) ./ ...
%              ( newtoffs - [2, newtoffs(1:end-1)] );
       
        temps = r(:,3)';
        slopes = r(:, 2)';
   
        B = [toffs; temps; slopes];
        
        D = [D, numel(toffs)];
        A = [A, B];
%         
    end
    
    csvwrite([prefix, '_warmingdata', num2str(i), '.csv'], A);
    csvwrite([prefix, '_warmingdataNumber', num2str(i), '.csv'], D');
    
end
csvwrite([prefix, '_slopestep.csv'],slopestep);


end