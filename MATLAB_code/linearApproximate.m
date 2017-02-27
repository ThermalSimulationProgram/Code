function table = linearApproximate(x, y, numseg, decreaseOrincrease)

if nargin < 3
    numseg = 4;
end

table = [];

if decreaseOrincrease > 0
    flag = 1;
else if decreaseOrincrease < 0
    flag = -1;
    else
        flag = 0;
    end
end

numexmpl = numel(y);

if numexmpl > numseg + 4
    nknots = (numseg + 1) * 2;
    [pp, ier] = BSFK(x, y, 2, nknots);
    while ( any(pp.coefs(:,1)*flag < 0) ||...
            any(pp.coefs(2:end,1) - pp.coefs(1:end-1,1)< 0) ) && ...
            nknots >= numseg
        nknots = nknots-1;
        [pp, ier] = BSFK(x, y, 2, nknots);
    end
    if any(pp.coefs(:,1)*flag < 0)
        error('wrong');
    end
    if ier
        breaks = pp.breaks(2:end);
        table(:,1) = breaks(:);
        endpointT = (pp.breaks(end) - pp.breaks(end-1)) * pp.coefs(end,1) + ...
            pp.coefs(end,2);
        table(:,3) = [pp.coefs(2:end,2); endpointT];
        table(:,2) = pp.coefs(:,1);
        
        fnplt(pp);hold on; plot(x, y);
        pause(0.5);
        clf;
        
    else
        error('failed');
    end
else
    table(:,1) = x(2:end)';
    table(:,2) = y(2:end)';
    slopes = y(2:end) - y(1:end-1);
    table(:,3) = slopes(:);
end
