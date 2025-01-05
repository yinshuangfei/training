
###
 # @Author: Alan Yin
 # @Date: 2024-07-20 09:52:59
 # @LastEditTime: 2024-07-20 10:11:02
 # @LastEditors: Alan Yin
 # @FilePath: /02_nas_mgmt/name_to_handle_at/test.sh
 # @Description:
 # // -*- mode:C; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
 # // vim: ts=8 sw=2 smarttab
 # Copyright (c) 2024 by HT706, All Rights Reserved.
###

# man 2 name_to_handle_at

echo 'Can you please think about it?' > cecilia.txt
./t_name_to_handle_at cecilia.txt > fh
./t_open_by_handle_at < fh

# Now  we  delete  and (quickly) re-create the file so that it has the same
# content and (by chance) the same inode.  Nevertheless, open_by_handle_at()
# recognizes that the original file referred to by the file handle no longer
# exists.
stat --printf="%i\n" cecilia.txt  # Display inode number
rm cecilia.txt
echo 'Can you please think about it?' > cecilia.txt
stat --printf="%i\n" cecilia.txt     # Check inode number
sudo ./t_open_by_handle_at < fh

