#!/usr/bin/env python3.8
# -*- coding: UTF-8 -*-

# import time
# from debug.debug import logger
# from daemon import runner
#
# class App():
#     def __init__(self):
#         self.stdin_path = '/dev/null'
#         self.stdout_path = '/dev/tty'
#         self.stderr_path = '/dev/tty'
#         self.pidfile_path = '/run/dcapp/multi-daemon.pid'
#         self.pidfile_timeout = 5
#
#     def run(self):
#         while True:
#             logger.debug('put')
#             time.sleep(1)
#
# app = App()
# daemon_runner = runner.DaemonRunner(app)
# daemon_runner.do_action()

from daemon import Daemon

aa = Daemon

aa.start("./ttt.pid")