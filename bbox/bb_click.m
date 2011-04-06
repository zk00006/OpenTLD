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


function bb = bb_click(tld,img)
% Create

if isfield(tld,'handle')
     h = get(gca,'Children'); delete(h(1:end-1));
    set(tld.handle,'cdata',img);
else
    imshow(img);
end
text(10,10,'Define bounding box and double click inside.','color','white');
h = imrect;
p = wait(h);
bb = [p(1); p(2);p(1)+p(3); p(2)+p(4)];
% [c,r,p] = impixel(img);
% if length(c) ~= 2,
%     bb = [];
%     return;
% end
% bb = [c(1); r(1); c(2); r(2)];