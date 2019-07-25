require 'rake/clean'
task default: :comcom
CLEAN.include "*.o"
CLEAN.include "comcom"
FLAGS = "-Wall -std=c11 -g"
CC = "gcc"
#CC = "clang"

desc "building..."
task :comcom do
  src_files = Rake::FileList.new "src/*.c"
  file "comcom" => src_files do |t|
    t.prerequisites.each do |f|
      sh "#{CC} -c #{FLAGS} #{f}"
    end
    obj_files = Rake::FileList.new "*.o"
    sh "#{CC} -o #{t.name} #{obj_files.join(' ')}"
  end
end
