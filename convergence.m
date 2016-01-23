dh1=h1(1:end-1)./h1(2:end);
dli=li(1:end-1)./li(2:end);
dh=h(1:end-1)./h(2:end);

convH=log(dh1)./log(dh)
convL=log(dli)./log(dh)
