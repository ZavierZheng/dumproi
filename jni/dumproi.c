/*
 * NOTICE:  All information contained herein is, and remains the property of Qeexo, Co.
 * The intellectual and technical concepts contained herein are proprietary to Qeexo, Co.
 * and may be covered by U.S. and foreign patents, patents in process, and are protected by
 * trade secret or copyright law. Dissemination of this information or reproduction of this
 * material is strictly forbidden unless prior written permission is obtained or is made
 * pursuant to a license agreement allowing such dissemination or reproduction from
 * Qeexo, Co.
 */

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>


#define DEFAULT_CAP_ROI_DATA_FILE "/sys/class/touch/cap_roi_data"
#define DEFAULT_DELAY_US 1000000
#define DEFAULT_ROI_ROWS 7
#define DEFAULT_ROI_COLUMNS 7

//#define REVERSE
//                                                 17         37
//          dump_cap_image(         buffer,        rows,      columns);
static void dump_cap_image(int16_t *cap_image, int height, int width) {
    int i, x = 0, y = 0;
    char cap_dump[width + 1];

    for (y = 0, i = 0; y < height; y++) {
        for (x = 0; x < width; x++, i++) {
            if (cap_image[i] > 64) {
                cap_dump[x] = '*';
            } else if (cap_image[i] > 10) {
                cap_dump[x] = '.';
            } else {
                cap_dump[x] = ' ';
            }
        }

        cap_dump[width] = '\0';
        printf("%s\n", cap_dump);
    }

    printf("\n\n");

    for (y = 0, i = 0; y < height; y++) {
        for (x = 0; x < width; x++, i++) {
            printf("%5d", cap_image[i]);
        }
        printf("\n");
    }

    printf("\n");
}

static int read_roi_data(char *latch_data_sysfs, void *buf, size_t bufsz) {
    int capimage_fd = open(latch_data_sysfs, O_RDONLY);
    if (capimage_fd < 0) {
        fprintf(stderr, "Could not open %s: %s\n", latch_data_sysfs, strerror(errno));
        return -1;
    }

    int ret = read(capimage_fd, buf, bufsz);
    if (ret < 0) {
        fprintf(stderr, "failed to read capacitive image: %s. Error: %s\n", latch_data_sysfs, strerror(errno));
        memset(buf, 0, bufsz);
        return -1;
    }
#ifdef REVERSE
    char * temp = buf;
    char tmp;
    int i;
    for(i = 0;i < bufsz/2;i++)
    {
        tmp = temp[2*i];
        temp[2*i] = temp[2*i+1];
        temp[2*i+1] = tmp;
    }
#endif

    close(capimage_fd);

    return 0;
}

int main(int argc, char **argv)
{
    int opt;
    char *latch_data_sysfs = DEFAULT_CAP_ROI_DATA_FILE;
    unsigned long us_delay = DEFAULT_DELAY_US;
    unsigned int rows = DEFAULT_ROI_ROWS;
    unsigned int columns = DEFAULT_ROI_COLUMNS;
    unsigned int clear_screen = 0;

    while ((opt = getopt(argc, argv, "r:l:d:c:he")) != -1) {
        switch (opt) {
        case 'l':
            latch_data_sysfs = optarg;
            break;
        case 'd':
            us_delay = strtol(optarg, NULL, 0);
            break;
        case 'r':
            rows = atoi(optarg);
            break;
        case 'c':
            columns = atoi(optarg);
            break;
        case 'e':
            clear_screen = 1;
            break;
        case 'h':
        default:
            goto error_opt;
        }
    }

    size_t data_size = rows * columns * sizeof(uint16_t);
    uint16_t *buffer = malloc(data_size);
    if(buffer == NULL) {
        fprintf(stderr, "Couldn't allocate buffer: %s\n", strerror(errno));
        goto error;
    }

    struct timeval start, stop;
    while(1) {
        if(clear_screen) {
            system("clear");
        }

        gettimeofday(&start, NULL);

        if(read_roi_data(latch_data_sysfs, buffer, data_size) < 0) {
            fprintf(stderr, "get_latch_data() error\n");
            goto error;
        }

        gettimeofday(&stop, NULL);
        float elapsed = (stop.tv_sec - start.tv_sec) * 1000.0 + (stop.tv_usec - start.tv_usec) / 1000.0;

        dump_cap_image(buffer, rows, columns);

        printf("ROI data read latency for driver level only: %3.2fms\n", elapsed);
        printf("--------------------------------------------------\n");

        usleep(us_delay);
    }

    free(buffer);
    return EXIT_SUCCESS;

error_opt:
    fprintf(stderr, "Usage: %s [-l <cap_roi_data_sysfs_file>]\n \
                [-d <usec_delay_between_reads>]\n \
                [-c <roi_columns>]\n \
                [-r <roi_rows>]\n", argv[0]);
    fprintf(stderr, "Example: %s -l " DEFAULT_CAP_ROI_DATA_FILE "\n \
                  -d 5000\n \
                  -c 7\n \
                  -r 7\n\n", argv[0]);
error:
    return EXIT_FAILURE;
}
