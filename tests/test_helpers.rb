# Helper methods for testing the 'checksum' utility#

# Copyright 2015 Ben Allen
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.


# Convert an ASCII-formatted hex dump into binary
def hex2bin(string)
    string.scan(/../).map { |x| x.to_i(16).chr }.join
end

# Create an ASCII-format dump of a binary string
def bin2hex(string)
    string.unpack("H*").join
end


# Run a binary data blob through the checksum utility
def checksum(message, args='')
    # Write data to temporary file
    filename = "test-test-test"
    File.open(filename, "wb") {|f| f.write message}

    # Run utility
    cmd = "./checksum #{args} #{filename}"
    stdout = `#{cmd}`.strip

    # Clean up by deleting the temporary file
    File.unlink(filename)

    return $?.success? ? stdout : nil
end
