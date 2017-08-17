FROM nginx:1.13.3

ADD dist/ /usr/share/nginx/html/
ADD v1/noobs /usr/share/nginx/html/v1/noobs

# noobs compatibily
ADD noobs-compatibility.sh /usr/local/bin/
RUN /usr/local/bin/noobs-compatibility.sh
