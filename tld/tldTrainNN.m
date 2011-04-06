% Copyright 2011 Zdenek Kalal
%
% This file is part of TLD.
% 
% TLD is free software: you can redistribute it and/or modify
% it under the terms of the GNU General Public License as published by
% the Free Software Foundation, either version 3 of the License, or
% (at your option) any later version.
% 
% TLD is distributed in the hope that it will be useful,
% but WITHOUT ANY WARRANTY; without even the implied warranty of
% MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
% GNU General Public License for more details.
% 
% You should have received a copy of the GNU General Public License
% along with TLD.  If not, see <http://www.gnu.org/licenses/>.

function tld = tldTrainNN(pEx,nEx,tld)

nP = size(pEx,2); % get the number of positive example 
nN = size(nEx,2); % get the number of negative examples

x = [pEx,nEx];
y = [ones(1,nP), zeros(1,nN)];

% Permutate the order of examples
idx = randperm(nP+nN); % 
if ~isempty(pEx)
    x   = [pEx(:,1) x(:,idx)]; % always add the first positive patch as the first (important in initialization)
    y   = [1 y(:,idx)];
end
   
for k = 1:1 % Bootstrap
   for i = 1:length(y)
       
       [conf1,~,isin] = tldNN(x(:,i),tld); % measure Relative similarity
       
       % Positive
       if y(i) == 1 && conf1 <= tld.model.thr_nn % 0.65
           if isnan(isin(2))
               tld.pex = x(:,i);
               continue;
           end
            %if isin(2) == size(tld.pex,2)
              %tld.pex = [tld.pex x(:,i)]; 
            %else
            tld.pex = [tld.pex(:,1:isin(2)) x(:,i) tld.pex(:,isin(2)+1:end)]; % add to model
            %end
       end
       
       % Negative
       if y(i) == 0 && conf1 > 0.5
           tld.nex = [tld.nex x(:,i)];
       end
   end
end