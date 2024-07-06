import pulumi
from pulumi_docker import Image, Container, DockerBuildArgs

image_name = "localhost/clamav_cont:v1.0"
image = Image("clamav-image",
              build=DockerBuildArgs(context = "/home/debian/scan/user_1/cont_1", platform="linux/amd64"),
              image_name=image_name,
              skip_push=True
)

container = Container("c1_user_x",
                      image=image.image_name,
                      name="c1_user_1",
                      mounts=[{
                          'target': '/scan/log',
                          'source': '/home/debian/scan/user_1/malware/test.py_4',
                          'type': 'bind'
                      }],
)
