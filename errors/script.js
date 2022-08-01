let os="Unknown";
if (navigator.userAgent.indexOf("Win")!=-1) os="Windows";
if (navigator.userAgent.indexOf("Mac")!=-1) os="MacOS";
if (navigator.userAgent.indexOf("X11")!=-1) os="UNIX";
if (navigator.userAgent.indexOf("Linux")!=-1) os="Linux";
document.getElementById("os").innerHTML = os;
document.getElementById("domain").innerHTML = window.location.hostname;

