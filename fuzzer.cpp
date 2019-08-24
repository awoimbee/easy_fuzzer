/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fuzzer.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: awoimbee <awoimbee@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/27 14:36:17 by awoimbee          #+#    #+#             */
/*   Updated: 2019/08/24 16:27:31 by awoimbee         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <fcntl.h>
#include <errno.h>
#include <vector>
#include <fstream>
#include <unistd.h>
#include <string.h>

// define max input file size as 1GiB
#define MAX_FSIZE (1024 * 1024 * 1024)

#define CORRUPT_NB_CHECKS 100UL
#define CORRUPT_NB_BYTES 500UL

#define PRINTF_COL	"\033[1;33m"
#define PRINTF_EOC	"\033[0m"

char	g_tmpFile[64];

void	quit(const char* errMsg) {
	if (errMsg)
		fprintf(stderr, "%s\n", errMsg);
	std::remove(g_tmpFile);
	fflush(stdout);
	fflush(stderr);
	exit(EXIT_FAILURE);
}

int		checkRet(int ret) {
	if (ret != 0 && ret != 256) {
		fprintf(stderr,
			"Child process terminated with return value %d\n"
			"You can check %s to investigate the issue\n", ret, g_tmpFile);
			fflush(stdout);
			fflush(stderr);
		return (1);
	}
	return (0);
}

int		main(int argc, const char** argv) {
	bool bytes_check = true;
	bool rand_check = true;

	/* INIT TMP FILE NAME */
	srandom(time(NULL));
	sprintf(g_tmpFile, "/tmp/%ld.fdestroy.tmp", random());

	/* USAGE */
	if (argc < 3) {
		fprintf(stdout,
			"usage: %1$s [--skip-byte | --skip-rand] <prog> <input_file> [<args>]\n"
			"\tExemples:\n"
			"\t\t %1$s ./corewar ./champ.cor '-dump 5'\n"
			"\t\t %1$s './corewar -dump 5' ./champ.cor '-n'\n"
			"\t\t %1$s './lem-in --print-lines <' ./big_map\n",
			argv[0]);
		return (EXIT_SUCCESS);
	}

	/* CHECK ARGS AND APPLY THEM */
	if (argc == 4) {
		if (!strcmp(argv[1], "--skip-byte"))
			bytes_check = false;
		else if (!strcmp(argv[1], "--skip-rand"))
			rand_check = false;
		else
			quit("Bad arguments.");
		memmove(&argv[1], &argv[2], 2 * sizeof(*argv));
	}


	/* READ INPUT FILE & CREATE g_tmpFile STREAM */
	int                  fd;
	size_t               size;
	std::vector<uint8_t> fileBuff;
	std::ofstream        out;

	fd = open(argv[2], O_RDONLY);
	if (fd == -1)
		quit(strerror(errno));
	size = lseek(fd, 0, SEEK_END);
	if (size == 0 || size > MAX_FSIZE)
		quit("Error: input file is empty or too big.");
	fileBuff.resize(size, 0);
	lseek(fd, 0, SEEK_SET);
	read(fd, &fileBuff[0], size); // we use read because c++ streams are hard to protect against /dev/*
	try {
		out.open(g_tmpFile, std::ios::trunc | std::ios::binary);
	}
	catch (std::ios_base::failure& e) {
		quit(e.what());
	}

	/* PREPARE THE MAGIC SAUCE */
	std::vector<char> cmd;

	if (argv[3] == nullptr)
		argv[3] = "";    // yolooo
	srandom(time(NULL));
	cmd.resize(
		strlen(argv[0])
		+ strlen(g_tmpFile)
		+ strlen(argv[2]));
	sprintf(&cmd[0], "%s %s %s", argv[1], g_tmpFile, argv[3]);

	/* SAUCE TIME */
	// pass the file to the selected programm, 1 byte at a time
	if (bytes_check) {
		printf(PRINTF_COL "Reading the file 1 byte at a time. This will loop %lu times: %s" PRINTF_EOC "\n",
			fileBuff.size() - 1,
			&cmd[0]);
		for (size_t i = 0; i < fileBuff.size(); ++i) {
			out << fileBuff[i];
			out.flush();
			fprintf(stdout,
				PRINTF_COL "################## Launching with %5lu Bytes ##################" PRINTF_EOC "\n",
				i + 1);
			if (checkRet(system(&cmd[0])))
				return (0);
		}
	}
	// corrupt the file
	if (rand_check) {
		printf("Corrupting the file. This will loop %lu times: %s\n",
			CORRUPT_NB_BYTES * CORRUPT_NB_CHECKS,
			&cmd[0]);
		for (size_t it = 0; it < CORRUPT_NB_CHECKS; ++it) {
			auto tmpBuff = fileBuff;
			for (size_t i = 0; i < CORRUPT_NB_BYTES; ++i) {
				printf(PRINTF_COL "################## loop %5lu ##################" PRINTF_EOC "\n", CORRUPT_NB_BYTES * it + i);
				tmpBuff[random() % tmpBuff.size()] = random();
				out.seekp(0);
				out.write(
					reinterpret_cast<const char*>(&tmpBuff[0]),
					tmpBuff.size());
				out.flush();
				if (checkRet(system(&cmd[0])))
					return (0);
			}
		}
	}
	out.close();
	std::remove(g_tmpFile);
	printf("Test successfull !\n");
	return (0);
}
