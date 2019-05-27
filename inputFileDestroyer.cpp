/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   inputFileDestroyer.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: awoimbee <awoimbee@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/27 14:36:17 by awoimbee          #+#    #+#             */
/*   Updated: 2019/05/27 16:51:44 by awoimbee         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
**	To compile:
**		g++ -std=c++14 -O3 -Wall -Wextra -Wpedantic OneByOne.cpp -o donateToAwoimbee
*/

#include <iostream>
#include <fcntl.h>
#include <errno.h>
#include <vector>
#include <fstream>
#include <unistd.h>

#define TMP_FILE "onebyone.tmp"
#define MAX_FSIZE (1024 * 1024 * 1024) // 1 gigabyte

#define CORRUPT_NB_CHECKS 100UL
#define CORRUPT_NB_BYTES 500UL

void	quit(
	const char* errMsg
) {
	if (errMsg)
		fprintf(stderr, "%s\n", errMsg);
	std::remove(TMP_FILE);
	fflush(stdout);
	fflush(stderr);
	exit(EXIT_FAILURE);
}

int		checkRet(
	int ret
) {
	if (ret != 0 && ret != 256) {
		fprintf(stderr,
			"Child process terminated with return value %d\n"
			"You can check " TMP_FILE " to investigate the issue\n", ret);
			fflush(stdout);
			fflush(stderr);
		return (1);
	}
	return (0);
}

int		main(
	int          argc,
	const char** argv
) {
	/* USAGE */
	if (argc != 3) {
		fprintf(stdout,
			"usage: %s <prog> <input_file> [<args>]\n"
			"\tExemples:\n"
			"\t\t %s ./corewar ./champ.cor '-dump 5'\n"
			"\t\t %s './corewar -dump 5' ./champ.cor '-n'\n"
			"\t\t %s './lem-in --print-lines <' ./big_map\n",
			argv[0], argv[0], argv[0], argv[0]); // I dont know of a better way
		return (EXIT_SUCCESS);
	}

	/* READ INPUT FILE & CREATE TMP_FILE STREAM */
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
	read(fd, &fileBuff[0], size); //we use read because c++ streams are hard to protect against /dev/*
	try {
		out.open(TMP_FILE, std::ios::trunc | std::ios::binary);
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
		+ strlen(TMP_FILE)
		+ strlen(argv[2]));
	sprintf(&cmd[0], "%s %s %s", argv[1], TMP_FILE, argv[3]);
	printf("Reading the file 1 byte at a time. This will loop %lu times: %s\n",
		fileBuff.size() - 1,
		&cmd[0]);

	/* SAUCE TIME */
	// pass the file to the selected programm, 1 byte at a time
	for (size_t i = 0; i < fileBuff.size(); ++i) {
		out << fileBuff[i];
		out.flush();
		fprintf(stdout,
			"################## Launching with %5lu Bytes ##################\n",
			i + 1);
		if (checkRet(system(&cmd[0])))
			return (0);
	}
	// corrupt the file
	printf("Corrupting the file. This will loop %lu times: %s\n",
		CORRUPT_NB_BYTES * CORRUPT_NB_CHECKS,
		&cmd[0]);
	for (size_t it = 0; it < CORRUPT_NB_CHECKS; ++it) {
		auto tmpBuff = fileBuff;
		for (size_t i = 0; i < CORRUPT_NB_BYTES; ++i) {
			printf("################## loop %5lu ##################\n", CORRUPT_NB_BYTES * it + i);
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
	out.close();
	printf("Test successfull !\n");
	return (0);
}
