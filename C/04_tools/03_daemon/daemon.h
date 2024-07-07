/*
 * @Author: Alan Yin
 * @Date: 2024-07-07 15:34:20
 * @LastEditTime: 2024-07-07 16:17:14
 * @LastEditors: Alan Yin
 * @FilePath: /training/windows_cifs/training/C/04_tools/03_daemon/daemon.h
 * @Description:
 * @// -*- mode:C; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
 * @// vim: ts=8 sw=2 smarttab
 * @Copyright (c) 2024 by Alan Yin, All Rights Reserved.
 */

#ifndef DAEMONIZE_H
#define DAEMONIZE_H

extern char *EXIT_REMOVED_PIDFILE;

int already_running(char *pidfile);

void simple_daemonize(void);
void daemonize(const char *cmd, void(*init_signal)(void));

#endif /** DAEMONIZE_H */