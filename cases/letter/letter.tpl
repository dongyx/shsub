<%+vars.tpl-%>
<%. "$(pwd)/utils.sh"-%>
<%w=80-%>
<%(-%>
A Letter from Shsub
<%)|center $w-%>
Hi, <%="$name"%>:
<%(-%>
Shsub is a template engine of the shell language, implemented in C.

**Key Features**

- Fast template compiling;
- Low memory footprint;
- Light-weight, containing only a standalone executable.
<%) | fold -w$(($w-8)) | awk '{print "\t" $0}'-%>
<%( %>- Shsub Developer<% ) | xargs -0 printf %${w}s%>
