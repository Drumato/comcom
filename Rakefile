require 'rake/clean'
task default: :comcom
CLEAN.include "*.o"

desc "building..."
task :comcom do
  src_files = Rake::FileList.new "src/*.c"
  file "comcom" => src_files do |t|
    t.prerequisites.each do |f|
      sh "gcc -c -Wall -std=c11 -g #{f}"
    end
    obj_files = Rake::FileList.new "*.o"
    sh "gcc -o #{t.name} #{obj_files.join(' ')}"
  end
end
