#!/usr/bin/ruby
# Script for testing the SHA256 algorithm using NIST test vectors

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

require_relative 'test_helpers'

# Calculate the SHA256 hash of a message string
def sha256(message)
    checksum(message, "-sha256")
end



# Run a set of known-answer tests from a set of NIST-formatted sample vectors
def sha256_kat(vector_file)
    # Define these here so that their scopes will cover all case statements
    failures = 0
    tests = 0
    target = ''
    msg = ''
    msg_string = ''
    msg_len = 0

    # Handle being run with pwd != this file's location
    if not File.exist?(vector_file)
        tmp = File.expand_path(vector_file, (File.dirname(__FILE__)))
        if not File.exist?(tmp)
            puts "Cannot locate file [#{vector_file}]"
        end
        vector_file = tmp
    end

    puts "Processing file #{vector_file} ..."

    # Process file, one line at a time
    IO.foreach(vector_file) do |line|
        # Remove comments and skip blank lines
        next if line.gsub(/[#].*$/,'').strip.empty?

        # A line without an equals sign on it is malformed
        next unless line.index(" = ")

        # Split line into label and data
        fields = line.strip.split
        next unless fields.length == 3
        case fields[0]
        when "[L"
            # We don't care about this particular line
            next
        when "Len"
            msg_len = fields[2].to_i
        when "Msg"
            msg_string = fields[2]
            if msg_len == 0
                msg = ""
            else
                # Convert to binary sequence
                msg = hex2bin(msg_string)
            end
        when "MD"
            target = '0x' + fields[2]
            # We have all our data now, so run the test
            tests += 1

            digest = sha256(msg)

            # Validate result
            if digest != target
                puts "Failed test case ##{tests}"
                failures += 1
            end

            # Reset variables for next test case
            target = ''
            msg = ''
            msg_string = ''
            msg_len = 0
        else
            next
        end
    end

    puts "Tests passed: #{tests - failures} / #{tests}"
end

# Run Monte Carlo tests from a set of NIST-formatted sample vectors
def sha256_mc(vector_file)
    tests = 0
    failures = 0
    seed = ''
    count = 0

    # Handle being run with pwd != this file's location
    if not File.exist?(vector_file)
        tmp = File.expand_path(vector_file, (File.dirname(__FILE__)))
        if not File.exist?(tmp)
            puts "Cannot locate file [#{vector_file}]"
        end
        vector_file = tmp
    end

    puts "Processing file #{vector_file} ..."
    
    IO.foreach(vector_file) do |line|
        # Remove comments and skip blank lines
        next if line.gsub(/[#].*$/,'').strip.empty?

        # A line without an equals sign on it is malformed
        next unless line.index(" = ")

        # Split line into label and data
        fields = line.strip.split
        next unless fields.length == 3
        case fields[0]
        when "[L"
            # We don't care about this particular line
            next
        when "Seed"
            seed = hex2bin(fields[2])
        when "COUNT"
            count = fields[2].to_i
        when "MD"
            digest = hex2bin(fields[2])

            # Spec method: requires large memory array (inefficient)
            # Try to optimize
            #INPUT: Seed  - A random seed 'n' bits long
            #{
            #    for (j=0; j<100; j++)
            #    {
            #        MD[0] = MD[1] = MD[2] = Seed;
            #        for (i=3; i<1003; i++)
            #        {
            #            #M[i] = MD[i-3] || MD[i-2] || MD[i-1];
            #            #MD[i] = SHA(M[i]);
            #            message = "three rounds ago" || "two rounds ago" || "last round";
            #            digest = SHA(message)
            #        }
            #        #MD[j] = Seed = MD[1002];
            #        MD[j] = Seed = "last digest";
            #        OUTPUT: MD[j]
            #    }
            #}
            # Run the next iterations (should be 100 in total)
            tests += 1
            md = []
            md[0] = seed
            md[1] = seed
            md[2] = seed
            (3...1003).each do |i|
                message = md[i-3] + md[i-2] + md[i-1]
                if message.length != (256 * 3 / 8)
                    puts "Error, message is wrong length (#{message.length})"
                    puts "message => #{bin2hex message}"
                    return
                end
                digest = sha256(message).sub(/^0[xX]/, '')
                md[i] = hex2bin(digest)
                if md[i].length != (256 / 8)
                    puts "Error, intermediate digest #{i} is wrong length (#{md[i].length})"
                    puts "  md[#{i}] => #{bin2hex md[i]}"
                    return
                end
            end
            seed = md[1002]
            md[count] = md[1002]

            # Check result
            if seed != hex2bin(digest)
                puts "Checkpoint #{count} mismatch:"
                puts "   expected => 0x#{digest}"
                puts "   got      => 0x#{bin2hex(seed)}"
                failures += 1
                break
            end
        else
            next
        end
    end
    puts "Tests passed: #{tests - failures} / #{tests}"
end


# Run known-answer tests
sha256_kat 'SHA256ShortMsg.rsp'
sha256_kat 'SHA256LongMsg.rsp'

# Run Monte-carlo tests
sha256_mc 'SHA256Monte.rsp'

#lines.map {|l|
#  a = ""
#  / = ([0-9a-f]+)/.match(x) {|m| m[1].split('').each_slice(2){|x,y| a += '\x' + x + y} }
#  if a.length == 0
#    x
#  else
#    a
#  end
#}

