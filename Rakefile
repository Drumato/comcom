require 'rake/clean'
task default: :comcom
["*.o","src/*.o","comcom","core","tmp","*.txt","a.out","*.s"].each do |f|
  CLEAN.include f

end

FLAGS = "-Wall -std=c11 -g"
CC = "gcc"
#CC = "clang"

def exec_shell(cc,flags,file_name)
  sh "#{cc} #{flags} #{file_name}"
end

desc "building..."
task :comcom do
  src_files = Rake::FileList.new "src/*.c"
  file "comcom" => src_files do |t|
    t.prerequisites.each do |f|
      exec_shell(CC,"-c "+FLAGS,f)
    end
    obj_files = Rake::FileList.new("*.o").join(' ')
    exec_shell(CC,"-o","#{t.name} "+obj_files)
  end
end
