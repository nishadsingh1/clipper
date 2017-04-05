FROM clipper/py-rpc:latest

MAINTAINER Nishad Singh <nishadsingh@gmail.com>

COPY predict_container.py /container/
COPY predict_container_setup.py /container/

COPY pywrencloudpickle.py /lib/

CMD ["python", "-u", "/container/predict_container_setup.py"]

# vim: set filetype=dockerfile:
