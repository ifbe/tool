#include <stdio.h>
#include <stdlib.h>
#include <alsa/asoundlib.h>
static char buffer[0x100000];

void main (int argc, char *argv[])
{
	int i, j;
	int err, fd;

	int channel;
	int buffer_frames = 1024;	//10ms
	unsigned int rate = 44100;

	snd_pcm_t *capture_handle;
	snd_pcm_hw_params_t *hw_params;
	snd_pcm_format_t format = SND_PCM_FORMAT_S16_LE;

	//
	if(argc<2)
	{
		printf("./a.out 2\n");
		exit(1);
	}

	//
	channel = argv[1][0] - 0x30;
	if((channel != 1) && (channel !=2))
	{
		printf("error@channel\n");
		exit(1);
	}

	//
	fd = open("test.pcm", O_CREAT|O_RDWR);
	if(fd <= 0)
	{
		printf("error@open\n");
		exit(1);
	}

	printf("@snd_pcm_open\n");
	err = snd_pcm_open (&capture_handle, "hw:1,0", SND_PCM_STREAM_CAPTURE, 0);
	if (err < 0) {
		printf("%s\n", snd_strerror (err));
		exit (1);
	}

	printf("@snd_pcm_hw_params_malloc\n");
	err = snd_pcm_hw_params_malloc (&hw_params);
	if (err < 0) {
		printf ("%s\n", snd_strerror (err));
		exit (1);
	}

	printf("@snd_pcm_hw_params_any\n");
	err = snd_pcm_hw_params_any (capture_handle, hw_params);
	if (err < 0) {
		printf ("%s\n", snd_strerror (err));
		exit (1);
	}

	printf("@snd_pcm_hw_params_set_access\n");
	err = snd_pcm_hw_params_set_access(
		capture_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED);
	if (err < 0) {
		printf ("%s\n", snd_strerror (err));
		exit (1);
	}

	printf("@snd_pcm_hw_params_set_format\n");
	err = snd_pcm_hw_params_set_format (capture_handle, hw_params, format);
	if (err < 0) {
		printf ("%s\n", snd_strerror (err));
		exit (1);
	}

	printf("@snd_pcm_hw_params_set_rate_near\n");
	err = snd_pcm_hw_params_set_rate_near (capture_handle, hw_params, &rate, 0);
	if (err < 0) {
		printf ("%s\n", snd_strerror (err));
		exit (1);
	}

	printf("@snd_pcm_hw_params_set_channels\n");
	err = snd_pcm_hw_params_set_channels (capture_handle, hw_params, channel);
	if (err < 0) {
		printf ("%s\n", snd_strerror (err));
		exit (1);
	}

	printf("@snd_pcm_hw_params\n");
	err = snd_pcm_hw_params (capture_handle, hw_params);
	if (err < 0) {
		printf ("%s\n", snd_strerror (err));
		exit (1);
	}
	snd_pcm_hw_params_free (hw_params);

	printf("@snd_pcm_hw_params\n");
	err = snd_pcm_prepare (capture_handle);
	if (err < 0) {
		printf ("%s\n", snd_strerror (err));
		exit (1);
	}

	printf("@snd_pcm_readi\n");
	for (i = 0; i < 200; ++i) {
		err = snd_pcm_readi (capture_handle, buffer, buffer_frames);
		if (err != buffer_frames) {
			printf ("%s\n", snd_strerror (err));
			exit (1);
		}
		printf("[%d]read %d\n", i, err);

		if(channel == 2)
		{
			err = write(fd, buffer, buffer_frames * 4);
		}
		else
		{
			for(j=0;j<buffer_frames;j++)
			{
				write(fd, buffer + j*2, 2);
				write(fd, (void*)&j, 2);
			}
		}
	}

	printf("@snd_pcm_close\n");
	snd_pcm_close (capture_handle);
	close(fd);
}