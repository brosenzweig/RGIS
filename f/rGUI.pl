#!/usr/bin/perl -w -C
####################################
# Created By: Fekete Andras        #
# Version V.2.0-2                  #
####################################

use Tk;
use strict;

my $sf;
my $bindir;
my $verbose;
my $autocmd;
my $msgs;
$bindir = $ENV{"HOME"} . "/.rGUIprefs"; # Use $bindir as a temp variable
if (!-e "$bindir") {
	$sf = 0;  # set '0' if you want long flags, or '1' for short flags
	$bindir = "/usr/local/app/ghaas/bin";
	while(!-e $bindir) { print "Please enter the location of the bin directory\n?> "; $bindir = <STDIN>; chomp($bindir); }
	$verbose = 1;
	$autocmd = 1;
	$msgs = 1;
} else {
	open(FILE,"$bindir") || die "Cannot open: $bindir";
	while(<FILE>) {
		chomp($_);
		if (/^\#/) {
		} elsif (/^BIN/) { $_ =~ s/BIN=//; $bindir = $_;
		} elsif (/^FLAGS/) { $_ =~ s/FLAGS=//; $sf = $_;
		} elsif (/^VERBOSE/) { $_ =~ s/VERBOSE=//; $verbose = $_;
		} elsif (/^AUTOCMD/) { $_ =~ s/AUTOCMD=//; $autocmd = $_;
		} elsif (/^MSGS/) { $_ =~ s/MSGS=//; $msgs = $_;
			if ($msgs == 0) {
				my $temp = $ENV{"HOME"} . "/.rGUIlog";
				if(open(LOGFILE,">$temp")) { select(LOGFILE); } else { print STDOUT "Couldn't open $temp, resorting to STDOUT"; select(STDOUT); $msgs = 0; }
			}
		} else { print "Warning: unrecognized line in ~/.rGUIprefs\n$_"; }
	}
	close(FILE);
}
my @trash;
my $input;
my @out_filetype;
$out_filetype[0] = "";
my $command = "Welcome to the program, this is where the command to be executed will be shown!";
my @commands;
my $c = 0;	# Counter for @commands

$commands[$c][0] = 2;
$commands[$c][1] = "";	# set program name to nothing
$commands[$c][2] = "";	# set argument to nothing

my %variables;
my $b = 0; # Counter for %variables

my @progArgs;
my $a = 0; # Counter for @progArgs

my @grdProgName;
my @grdProgIn;
my @grdProgOut;
my $grdProgMax = 0;
my @netProgName;
my @netProgIn;
my @netProgOut;
my $netProgMax = 0;
my @pntProgName;
my @pntProgIn;
my @pntProgOut;
my $pntProgMax = 0;
my @tblProgName;
my @tblProgIn;
my @tblProgOut;
my $tblProgMax = 0;

&parse_prog();
my $mw = MainWindow->new;
$mw->title("Shell Maker");
my $menu = $mw->Frame(-relief => 'ridge', -borderwidth => 2)->pack(-fill => 'x');
my $filebutton = $menu->Menubutton(-text=>"File", -underline => 0)->pack(-side => 'left');
my $filemenu = $filebutton->Menu();
$filebutton->configure(-menu=>$filemenu);
$filemenu->command(-label => "Load", -underline => 0, -command => sub {
	if ($c != 0) {
		my $warn = "You have something open, make sure you have saved your work before opening a new file!\nAre you sure you want to continue?";
		my $warning = MainWindow->new;
		$warning->title("Question");
		$warning->Label(-text => $warn)->pack;
		$warning->Button(-text => "No", -command => sub { $warning->destroy; })->pack(-side => 'bottom');
		$warning->Button(-text => "Yes", -command => sub { $warning->destroy; &do_load; })->pack(-side => 'bottom');
	} else { &do_load; }
});
$filemenu->command(-label => "Save", -underline => 0, -command => \&do_save);
$filemenu->command(-label => "Make Shell Script", -command => \&do_shell_script);
$filemenu->separator;
$filemenu->command(-label => "Exit", -underline => 1, -command => \&do_quit);
my $varbutton = $menu->Menubutton(-text=>"Variable Func", -underline => 0)->pack(-side => 'left');
my $varmenu = $varbutton->Menu();
$varbutton->configure(-menu=>$varmenu);
$varmenu->command(-label => "Add New", -command => \&do_add_var);
$varmenu->command(-label => "Modify Selected", -command => \&do_mod_var);
$varmenu->command(-label => "Delete Selected", -command => \&do_del_var);
my $combutton = $menu->Menubutton(-text=>"Command Func", -underline => 0)->pack(-side => 'left');
my $commenu = $combutton->Menu();
$combutton->configure(-menu=>$commenu);
$commenu->command(-label => "Add New", -command => \&do_add_com);
$commenu->command(-label => "Modify Selected", -command => \&do_mod_com);
$commenu->command(-label => "Delete Last", -command => \&do_del_com);
my $optionsbutton = $menu->Button(-text => "Options", -underline => 0, -command => \&set_options)->pack(-side => 'right');
my $docmdbtn = $menu->Button(-text => "Do Command", -underline => 3, -command => \&do_command)->pack(-side => 'right');
if ($autocmd) { $docmdbtn->configure(-state => 'disabled'); }
my $main_frame = $mw->Frame(-relief => 'flat', -borderwidth => 0)->pack(-fill => 'x');
my $var_frame = $main_frame->Frame(-relief => 'ridge', -borderwidth => 2)->pack(-side => 'left', -fill => 'x', -expand => 1);
$var_frame->Label(-text => "Variables:")->pack(-anchor => 'n');
my $lb_var = $var_frame->Scrolled("Listbox", -scrollbars => 'ose', -selectmode => "single")->pack(-fill => 'x', -expand => 1);
my $com_frame = $main_frame->Frame(-relief => 'ridge', -borderwidth => 2)->pack(-side => 'right', -fill => 'x', -expand => 1);
$com_frame->Label(-text => "Commands:")->pack(-anchor => 'n');
my $lb_com = $com_frame->Scrolled("Listbox", -scrollbars => 'ose', -selectmode => "single")->pack(-fill => 'x', -expand => 1);
my $cmd = $mw->Frame(-relief => 'ridge', -borderwidth => 2)->pack(-fill => 'both', -expand => 1);
my $t = $cmd->Scrolled("Text", -height => 10, -scrollbars => 'ose')->pack(-fill => 'both', -expand => 1);
$t->insert('end',$command);
$cmd->focus;
$mw->update;	# prevents lost characters
#&do_warn("This is an Unregistered Beta version!\nPlease update and/or register!");
MainLoop;

sub do_add_var {
	my $mw_addvar = MainWindow->new;
	my $name = "";
	my $value = "";
	$mw_addvar->title("Define variables!");
	$mw_addvar->Label(-text => "Alias")->grid(
		$mw_addvar->Label(-text => "Value"), -padx => 10);
	my $focus = $mw_addvar->Entry(-textvariable => \$name)->grid(
		$mw_addvar->Entry(-textvariable => \$value) );
	$mw_addvar->Button(-text => "Cancel", -command => sub { $mw_addvar->destroy; })->grid(
		$mw_addvar->Button(-text => "Ok", -command => sub {
			if (($name eq "") || ($value eq "")) { &do_warn("You must supply an Alias AND a Value!");
			} else {
				$b++;
				$name = uc($name);
				if ($name eq "ANSWER") { $value = 42; &do_warn("The Answer to Life, The Universe and Everything is...42\n\nWell, you ask, what is the question?\n\"I can't do that,\" Deep Thought says. \"That would require another and even larger computer that he will have to build and it will search for The Question. I will call this computer: Earth.\""); }
				$variables{$name} = $value;
				&do_variable;
			}
			$mw_addvar->destroy;
		}), -padx => 10);
	$focus->focus;
}

sub do_mod_var {
	my $x = 0;
	my $name = $lb_var->get('active');
	if (!defined($name)) { &do_warn("Nothing selected!"); return; }
	my $value = $variables{$name};
	my $mw_addvar = MainWindow->new;
	$mw_addvar->title("REdefine variables!");
	$mw_addvar->Label(-text => "Alias")->grid(
		$mw_addvar->Label(-text => "Value"), -padx => 10);
	my $focus = $mw_addvar->Entry(-textvariable => \$name, -state => 'disabled')->grid(
		$mw_addvar->Entry(-textvariable => \$value) );
	$mw_addvar->Button(-text => "Cancel", -command => sub { $mw_addvar->destroy; })->grid(
		$mw_addvar->Button(-text => "Ok", -command => sub { $name = uc($name); $variables{$name} = $value; &do_variable; $mw_addvar->destroy; }), -padx => 10);
	$focus->focus;
}

sub do_del_var {
	my $name = $lb_var->get('active');
	my $fx = $c;
	my $fy = 0;
	if (defined($name)) {
		for (my $x = 0; $x < $fx; $x++) {
			$fy = $commands[$x][0];
			for (my $z = 2; $z <= $fy; $z++) { if ($commands[$x][$z] eq "\${$name}") { $fx = $x; $fy = $z; } }
		}
		if ($fx != $c) { &do_warn("Variable in use ($commands[$fx][1])"); } else { delete $variables{$name}; $b--; &do_variable; }
	} else { &do_warn("Nothing to erase!"); }
}

sub do_variable {
	$lb_var->delete(0, 'end');
	foreach $_ (keys %variables) { $lb_var->insert('end', $_); }
}

sub do_add_com {
	my $mw2_program_type = "";
	my $mw2_program = "";
	my $mw2_programs = [ '' ];
	my $mw2 = MainWindow->new;
	$mw2->title("Select operation!");
	my $mw2_menu = $mw2->Frame(-relief => 'ridge', -borderwidth => 2)->pack(-fill => 'x');
	my $menu2 = $mw2_menu->Optionmenu(-variable => \$mw2_program, -options => $mw2_programs)->pack(-side => 'left');
	my $menu = $mw2_menu->Optionmenu(-variable => \$mw2_program_type, -options => ['grd', 'net', 'pnt', 'tbl'], -command => sub {
		if ($mw2_program_type eq "grd") {
			if ($out_filetype[$c - 1] eq "") { $mw2_programs = [ @grdProgName ];
			} else {
				my @trash;
				my $b = 0;
				for (my $a = 0; $a <= $grdProgMax; $a++) { if ($grdProgIn[$a] eq $out_filetype[$c - 1]) { $trash[$b] = $grdProgName[$a]; $b++; } }
				$mw2_programs = [ @trash ];
			}
		} elsif ($mw2_program_type eq "net") {
			if ($out_filetype[$c - 1] eq "") { $mw2_programs = [ @netProgName ];
			} else {
				my @trash;
				my $b = 0;
				for (my $a = 0; $a <= $netProgMax; $a++) { if ($netProgIn[$a] eq $out_filetype[$c - 1]) { $trash[$b] = $netProgName[$a]; $b++; } }
				$mw2_programs = [ @trash ];
			}
		} elsif ($mw2_program_type eq "pnt") {
			if ($out_filetype[$c - 1] eq "") { $mw2_programs = [ @pntProgName ];
			} else {
				my @trash;
				my $b = 0;
				for (my $a = 0; $a <= $pntProgMax; $a++) { if ($pntProgIn[$a] eq $out_filetype[$c - 1]) { $trash[$b] = $pntProgName[$a]; $b++; } }
				$mw2_programs = [ @trash ];
			}
		} elsif ($mw2_program_type eq "tbl") {
			if ($out_filetype[$c - 1] eq "") { $mw2_programs = [ @tblProgName ];
			} else {
				my @trash;
				my $b = 0;
				for (my $a = 0; $a <= $tblProgMax; $a++) { if ($tblProgIn[$a] eq $out_filetype[$c - 1]) { $trash[$b] = $tblProgName[$a]; $b++; } }
				$mw2_programs = [ @trash ];
			}
		} else { $mw2_programs = [ '' ]; }
		$menu2->options($mw2_programs); } )->pack(-side => 'right');
	$menu->focus;
	my $mw2_buttons = $mw2->Frame(-relief => 'ridge', -borderwidth => 2)->pack(-fill => 'x');
	$mw2_buttons->Button(-text => "Cancel", -command => sub { $mw2->destroy; })->pack(-side => 'left');
	$mw2_buttons->Button(-text => "Ok", -command => sub {
		my $out = "";
		if ($mw2_program_type eq "grd") {
			for (my $a = 0; $a <= $grdProgMax; $a++) { if ($grdProgName[$a] eq $mw2_program) { $out = $grdProgOut[$a]; } }
		} elsif ($mw2_program_type eq "net") {
			for (my $a = 0; $a <= $netProgMax; $a++) { if ($netProgName[$a] eq $mw2_program) { $out = $netProgOut[$a]; } }
		} elsif ($mw2_program_type eq "pnt") {
			for (my $a = 0; $a <= $pntProgMax; $a++) { if ($pntProgName[$a] eq $mw2_program) { $out = $pntProgOut[$a]; } }
		} elsif ($mw2_program_type eq "tbl") {
			for (my $a = 0; $a <= $tblProgMax; $a++) { if ($tblProgName[$a] eq $mw2_program) { $out = $tblProgOut[$a]; } }
		}
		if ($mw2_program eq "") { &do_warn("No program selected!"); } else { &do_mw3($mw2_program, $out); }
		$mw2->destroy;
	})->pack(-side => 'right');
}

sub do_mw3 {
	my $mw2_program = shift;
	&do_getOpt($mw2_program);
	my $out = shift;
	my @selectedArg;
	my $mw3 = MainWindow->new;
	$mw3->title("Select program options!");
	for(my $y = 0; $y < $a; $y++){
		$selectedArg[$y][0] = 0;
		$selectedArg[$y][1] = "";
		if (($progArgs[1][$y] eq  "Verbose") || ($progArgs[1][$y] eq "verbose")) { $selectedArg[$y][0] = 1; }
			if ($b) {
				$mw3->Label(-text => $progArgs[1][$y])->grid(
					$mw3->Checkbutton(-text => "Use", -onvalue => 1, -offvalue => 0, -variable => \$selectedArg[$y][0]),
					$selectedArg[$y][2] = $mw3->Optionmenu(-variable => \$selectedArg[$y][1]),
					$mw3->Entry(-textvariable => \$selectedArg[$y][1]) );
				foreach $_ (keys %variables) { $selectedArg[$y][2]->addOptions(["$_\($variables{$_}\)" => "\$\{$_\}"]); }
				$selectedArg[$y][2]->addOptions(['TextBox' => '']);
			} else {
				$mw3->Label(-text => $progArgs[1][$y])->grid(
					$mw3->Checkbutton(-text => "Use", -onvalue => 1, -offvalue => 0, -variable => \$selectedArg[$y][0]),
					$mw3->Entry(-textvariable => \$selectedArg[$y][1]) ); 
			}
	}
	$mw3->Button(-text => "Cancel", -command => sub { $mw3->destroy; })->grid( "x", "x",
		$mw3->Button(-text => "Ok", -command => sub {
			$commands[$c][1] = $mw2_program;
			for (my $y = 0; $y < $a; $y++) {
				if ($selectedArg[$y][0] == 1) {
					my $d = $commands[$c][0];
					if ($sf) { $commands[$c][$d] = "-" . $progArgs[0][$y]; } else { $commands[$c][$d] = "--" . $progArgs[1][$y]; }
					$d++;
					$commands[$c][$d] = $selectedArg[$y][1];
					$d++;
					$commands[$c][0] = $d;
				}
			}
			$commands[$c][0] = $commands[$c][0] - 1;
			$out_filetype[$c] = $out;
			print "OUTFILE: $out_filetype[$c]\n";
			$c++;
			$commands[$c][0] = 2;
			$commands[$c][1] = "";	# set program name to nothing
			$commands[$c][2] = "";	# set argument to nothing
			$out_filetype[$c] = "";
			if ($autocmd) { &do_command; }
			$mw3->destroy;
		})
	);
}

sub do_mod_com {
	my @selectedArg;
	my $x = 0;
	my $mw2_program = $lb_com->get('active');
	if (!defined($mw2_program)) { &do_warn("Nothing selected!"); return; }
	while ($mw2_program ne $commands[$x][1] && $x < $c) { $x++; }
	&do_getOpt($commands[$x][1]);
	my $d = 2;
	my $test = $commands[$x][$d];
	$test =~ s/^--//;
	for (my $y = 0; $y < $a; $y++) {
		if ($progArgs[1][$y] eq $test) {
				$selectedArg[$y][0] = 1;
				$d++;
				$selectedArg[$y][1] = $commands[$x][$d];
				$d++;
				if ($d < $commands[$x][0]) {
					$test = $commands[$x][$d];
					$test =~ s/^--//;
				} else { $test = "blah"; }
		} else { $selectedArg[$y][0] = 0; }
	}
	my $mw3 = MainWindow->new;
	$mw3->title("Select program options!");
	for(my $y = 0; $y < $a; $y++){
		if (($progArgs[1][$y] eq  "Verbose") || ($progArgs[1][$y] eq "verbose")) { $selectedArg[$y][0] = 1; }
			if ($b) {
				my $temp = $selectedArg[$y][1];
				$mw3->Label(-text => $progArgs[1][$y])->grid(
					$mw3->Checkbutton(-text => "Use", -onvalue => 1, -offvalue => 0, -variable => \$selectedArg[$y][0]),
					$selectedArg[$y][2] = $mw3->Optionmenu(-variable => \$selectedArg[$y][1]),
					$mw3->Entry(-textvariable => \$selectedArg[$y][1]) );
				foreach $_ (keys %variables) { $selectedArg[$y][2]->addOptions(["$_\($variables{$_}\)" => "\$\{$_\}"]); }
				$selectedArg[$y][2]->addOptions(['TextBox' => '']);
				$selectedArg[$y][1] = $temp;
			} else {
				$mw3->Label(-text => $progArgs[1][$y])->grid(
					$mw3->Checkbutton(-text => "Use", -onvalue => 1, -offvalue => 0, -variable => \$selectedArg[$y][0]),
					$mw3->Entry(-textvariable => \$selectedArg[$y][1]) ); 
			}
	}
	$mw3->Button(-text => "Cancel", -command => sub { $mw3->destroy; })->grid( "x",
		$mw3->Button(-text => "Ok", -command => sub {
		$commands[$x][0] = 2;
		for (my $y = 0; $y < $a; $y++) {
			if ($selectedArg[$y][0] == 1) {
				my $d = $commands[$x][0];
				if ($sf) { $commands[$x][$d] = "-" . $progArgs[0][$y]; } else { $commands[$x][$d] = "--" . $progArgs[1][$y]; }
				$d++;
				$commands[$x][$d] = $selectedArg[$y][1];
				$d++;
				$commands[$x][0] = $d;
			}
		}
		$commands[$x][0] = $commands[$x][0] - 1;
		if ($autocmd) { &do_command; }
		$mw3->destroy;
		})
	);
}

sub do_del_com {
	$lb_com->delete('end');
	if ($c != 0) { $c--; } else { &do_warn("Nothing to erase!"); }
	$commands[$c][0] = 2;
	$commands[$c][1] = "";	# set program name to nothing
	$commands[$c][2] = "";	# set argument to nothing
	$out_filetype[$c] = "";
	print "OUTFILE: $out_filetype[$c - 1]\n";
	if ($autocmd) { &do_command; }
}

sub do_quit {
	&do_command;
	if ($verbose) {
		print "Execute:\t";
		if ($command ne "Welcome to the program, this is where the command to be executed will be shown!" ) { print "\'$command\'\n"; } else { print "Nothing to do!\n"; }
	}
	close(LOGFILE);
	$mw->destroy;
	exit;
}

sub do_load {
	my $step = 1;
	$c = 0;
	$commands[$c][0] = 0;
	$commands[$c][1] = "";
	$commands[$c][2] = "";
	foreach $_ (keys %variables) { delete $variables{$_}; }
	my $file = $mw->getOpenFile(-filetypes => [['ShellMaker Files','.sme']], -defaultextension => '.sme', -title => 'Load ShellMaker File');
	if (defined($file)) {
		if ($verbose == 3) { print "file: $file\n"; }
		open(INFILE,$file) || die ("Can't open specified file!");
		while (<INFILE>) {
			chomp;
			if (/#####vars#####/) { $step = 2; } elsif (/#####end#####/) { $step = 0; }
			if ($step == 1) {
				my @crap = split(/\t/,$_);
				for (my $x = 0; $x <= $#crap; $x++) { $commands[$c][$x] = $crap[$x]; }
				$c++;
				$commands[$c][0] = 0;
				$commands[$c][1] = "";
				$commands[$c][2] = "";
			} elsif ($step == 3) {
				my @crap = split(/\t/,$_);
				$variables{$crap[0]} = $crap[1];
			} elsif ($step == 2) { $step = 3;
			}
		}
		close(INFILE);
		&do_variable;
		&do_command;
	} else { print "File undefined!\n"; }
}

sub do_save {
	my $file = $mw->getSaveFile(-filetypes => [['ShellMaker Files','.sme']], -defaultextension => '.sme', -title => 'Save to a ShellMaker File');
	if (defined($file)) {
		if(open(OUTFILE,">$file")) {
			for (my $x = 0; $x < $c; $x++) {
				for (my $d = 0; $d < $commands[$x][0]; $d++) {
					if ($d == 0) { print OUTFILE "$commands[$x][$d]"; } else { print OUTFILE "\t$commands[$x][$d]"; }
				}
				print OUTFILE "\n";
			}
			print OUTFILE "#####vars#####\n";
			foreach $_ (keys %variables) { print OUTFILE "$_\t$variables{$_}\n"; }
			print OUTFILE "#####end#####\n";
			close(OUTFILE);
		} else { &do_warn("Unable to open file for writing: $file!\n"); }
	} else { print "File undefined!\n"; }
}

sub do_shell_script {
	my $file;
	my $mw_shell = MainWindow->new;
	$mw_shell->title("Enter Filename!");
	$mw_shell->Label(-text => "Enter Filename!")->grid(
		my $focus = $mw_shell->Entry(-textvariable => \$file) );
	$mw_shell->Button(-text => "Cancel", -command => sub { $mw_shell->destroy; })->grid(
		$mw_shell->Button(-text => "Ok", -command => sub {
			if (defined($file)) {
				if(open(OUTFILE,">$file")) {
					print OUTFILE "#!/bin/sh\n";
					print OUTFILE "#####\tvars\t#####\n";
					foreach $_ (keys %variables) { print OUTFILE "$_ = $variables{$_}\n"; }
					print OUTFILE "\n#####\tmain\t#####\n\n";
					&do_command;
					print OUTFILE "$command\n";
					close(OUTFILE);
				} else { &do_warn("Unable to open file for writing: $file!\n"); }
			} else { print "File undefined!\n"; }
			$mw_shell->destroy;
		})
	);
	$focus->focus;
}

sub do_command {
	if ($c != 0) {
		my $x = 0;
		$lb_com->delete(0, 'end');
		$command = "";
		for($x = 0; $x < $c; $x++) {
			my $noverbose = 1;
			if ($x != 0) { $command = $command . " |\\\n"; }
			for (my $d = 1; $d < $commands[$x][0]; $d++) {
				if ($d == 1) { $command = $command . $commands[$x][$d]; } else { $command = $command . " " . $commands[$x][$d]; }
				if ($noverbose == 1) {
					if ($sf) { if ($commands[$x][$d] =~ /-V/) { $noverbose = 0; } } else { if ($commands[$x][$d] =~ /--verbose/) { $noverbose = 0; } }
				}
			}
			if (($verbose == 3) && ($noverbose)) { if ($sf) { $command = $command . "-V"; } else { $command = $command . " --verbose"; } }
			$lb_com->insert('end', $commands[$x][1]);
		}
		if ($verbose >= 2) { print "$command"; }
		if ($command ne "") { if ($verbose >= 2) { print "\nCommand: '$command'\n"; } } else { $command = "Welcome to the program, this is where the command to be executed will be shown!"; }
		$t->delete("1.0",'end');
		$t->insert('end',$command);
	} else {
		$lb_com->delete(0, 'end');
		$command = "Welcome to the program, this is where the command to be executed will be shown!";
		$t->delete("1.0",'end');
		$t->insert('end',$command);
	}
}

sub do_warn {
	my $warn = shift;
	my $warning = MainWindow->new;
	$warning->title("Warning!");
	$warning->Label(-text => $warn)->pack;
	my $focus = $warning->Button(-text => "Ok", -command => sub { $warning->destroy; })->pack(-side => 'bottom');
	$focus->focus;
}

sub do_error {
	my $warn = shift;
	my $warning = MainWindow->new;
	$warning->title("Error!");
	$warning->Label(-text => $warn)->pack;
	$warning->Button(-text => "Ok", -command => sub { $warning->destroy; })->pack(-side => 'bottom');
}

sub do_getOpt {
	my $program = shift;
	my $intype = "";
	$a = 0;
	system ("$bindir/$program -h >& aa.out");
	open(ARGS,"aa.out") || die "Couldn't execute $program";
	while(<ARGS>) {
		chomp;
		if (/^[ ]*-/) {
			$_ =~ s/^[ ]*-//;
			$_ =~ s/=>*//; #delete comments
			$_ =~ s/=[ ]*//; #delete comments
			my ($short, $long) = split (/[ ]./,$_);
			my ($trash , $intype) = split(/\[/,$_);
			$short =~ s/,//;
			$long =~ s/^-//;
			if (defined($intype)) { $intype =~ s/\]//; $progArgs[2][$a] = $intype; } else { $progArgs[2][$a] = undef; }
			$progArgs[0][$a] = $short;
			$progArgs[1][$a] = $long;
			if ($verbose >= 3) { if (defined($progArgs[2][$a])) { print "$program: $progArgs[0][$a] : $progArgs[1][$a] : $progArgs[2][$a]\n"; } else { print "$program: $progArgs[0][$a] : $progArgs[1][$a] : \n"; } }
			$a++;
		}
	}
	$a--;
	close(ARGS);
	system ("rm aa.out");
}

sub parse_prog {
	print "Processing available programs. Please be patient,\nthis *should* take less than 5 seconds on a 2.0Ghz CPU.\n";
	my $a = 0;
	system ("ls $bindir/grd* >& aa.out");
	open(PROG,"aa.out") || die "Couldn't find any files \'grd*\'";
	while(<PROG>) {
		chomp;
		s/$bindir\///;
		$grdProgName[$a] = $_;
		$grdProgIn[$a] = "none";
		$grdProgOut[$a] = "none";
		system ("$bindir/$grdProgName[$a] -h >& bb.out");	
		open(ARGS, "bb.out") || die "Couldn't execute $bindir/$grdProgName[$a]";
		chomp($_ = <ARGS>);
		my @trash = split(/[ ]/,$_);
		for (my $b = 0; $b <= $#trash; $b++) {
			if ($trash[$b] eq "<input") { $grdProgIn[$a] = $trash[$b+1]; $grdProgIn[$a] =~ s/[>]//; $b++; }
			if ($trash[$b] eq "<output") { $grdProgOut[$a] = $trash[$b+1]; $grdProgOut[$a] =~ s/[>]//; $b++; }
		}
		if (($grdProgIn[$a] eq "none") || ($grdProgOut[$a] eq "none")) { print "$grdProgName[$a]\t$grdProgIn[$a]\t$grdProgOut[$a]\n"; } else { $a++; }
		close(ARGS);
		system ("rm bb.out");
	}
	$grdProgMax = $a - 1;
	close(PROG);
	system ("rm aa.out");
	$a = 0;
	system ("ls $bindir/net* >& aa.out");
	open(PROG,"aa.out") || die "Couldn't find any files \'net*\'";
	while(<PROG>) {
		chomp;
		s/$bindir\///;
		$netProgName[$a] = $_;
		$netProgIn[$a] = "none";
		$netProgOut[$a] = "none";
		system ("$bindir/$netProgName[$a] -h >& bb.out");	
		open(ARGS, "bb.out") || die "Couldn't execute $bindir/$netProgName[$a]";
		chomp($_ = <ARGS>);
		my @trash = split(/[ ]/,$_);
		for (my $b = 0; $b <= $#trash; $b++) {
			if ($trash[$b] eq "<input") { $netProgIn[$a] = $trash[$b+1]; $netProgIn[$a] =~ s/[>]//; $b++; }
			if ($trash[$b] eq "<output") { $netProgOut[$a] = $trash[$b+1]; $netProgOut[$a] =~ s/[>]//; $b++; }
		}
		if (($netProgIn[$a] eq "none") || ($netProgOut[$a] eq "none")) { print "$netProgName[$a]\t$netProgIn[$a]\t$netProgOut[$a]\n"; } else { $a++; }
		close(ARGS);
		system ("rm bb.out");
	}
	$netProgMax = $a - 1;
	close(PROG);
	system ("rm aa.out");
	$a = 0;
	system ("ls $bindir/pnt* >& aa.out");
	open(PROG,"aa.out") || die "Couldn't find any files \'pnt*\'";
	while(<PROG>) {
		chomp;
		s/$bindir\///;
		$pntProgName[$a] = $_;
		$pntProgIn[$a] = "none";
		$pntProgOut[$a] = "none";
		system ("$bindir/$pntProgName[$a] -h >& bb.out");	
		open(ARGS, "bb.out") || die "Couldn't execute $bindir/$pntProgName[$a]";
		chomp($_ = <ARGS>);
		my @trash = split(/[ ]/,$_);
		for (my $b = 0; $b <= $#trash; $b++) {
			if ($trash[$b] eq "<input") { $pntProgIn[$a] = $trash[$b+1]; $pntProgIn[$a] =~ s/[>]//; $b++; }
			if ($trash[$b] eq "<output") { $pntProgOut[$a] = $trash[$b+1]; $pntProgOut[$a] =~ s/[>]//; $b++; }
		}
		if (($pntProgIn[$a] eq "none") || ($pntProgOut[$a] eq "none")) { print "$pntProgName[$a]\t$pntProgIn[$a]\t$pntProgOut[$a]\n"; } else { $a++; }
		close(ARGS);
		system ("rm bb.out");
	}
	$pntProgMax = $a - 1;
	close(PROG);
	system ("rm aa.out");
	$a = 0;
	system ("ls $bindir/tbl* >& aa.out");
	open(PROG,"aa.out") || die "Couldn't find any files \'tbl*\'";
	while(<PROG>) {
		chomp;
		s/$bindir\///;
		$tblProgName[$a] = $_;
		$tblProgIn[$a] = "none";
		$tblProgOut[$a] = "none";
		system ("$bindir/$tblProgName[$a] -h >& bb.out");	
		open(ARGS, "bb.out") || die "Couldn't execute $bindir/$tblProgName[$a]";
		chomp($_ = <ARGS>);
		my @trash = split(/[ ]/,$_);
		for (my $b = 0; $b <= $#trash; $b++) {
			if ($trash[$b] eq "<input") { $tblProgIn[$a] = $trash[$b+1]; $tblProgIn[$a] =~ s/[>]//; $b++; }
			if ($trash[$b] eq "<output") { $tblProgOut[$a] = $trash[$b+1]; $tblProgOut[$a] =~ s/[>]//; $b++; }
		}
		if (($tblProgIn[$a] eq "none") || ($tblProgOut[$a] eq "none")) { print "$tblProgName[$a]\t$tblProgIn[$a]\t$tblProgOut[$a]\n"; } else { $a++; }
		close(ARGS);
		system ("rm bb.out");
	}
	$tblProgMax = $a - 1;
	close(PROG);
	system ("rm aa.out");
	print "GRD: $grdProgMax\nNET: $netProgMax\nPNT: $pntProgMax\nTBL: $tblProgMax\n";
}

sub set_options {
	my $optwin = MainWindow->new;
	$optwin->title("Shell Maker Options");
	my $options = $optwin->Frame(-relief => 'ridge', -borderwidth => 2)->pack(-fill => 'x');
	$options->Label(-text => "All changes made herein, will be effective for the life of the program.")->pack(-side => 'top');
	$options->Label(-text => "Only if they are saved, will they be used upon next startup.")->pack(-side => 'bottom');
	$options = $optwin->Frame(-relief => 'ridge', -borderwidth => 2)->pack(-fill => 'x');
	$options->Button(-text => "Close(no save)", -command => sub { $optwin->destroy; })->pack(-side => 'left');
	$options->Button(-text => "Save + Close", -command => sub { &save_prefs; $optwin->destroy; })->pack(-side => 'left');
	$options->Entry(-textvariable => \$bindir)->pack(-side => 'right');
	$options = $optwin->Frame(-relief => 'ridge', -borderwidth => 2)->pack(-fill => 'x');
	$options->Radiobutton(-text => "Yes", -value => "1", -variable => \$autocmd, -command => sub { $docmdbtn->configure(-state => 'disabled'); })->pack(-side => 'right');
	$options->Radiobutton(-text => "No", -value => "0", -variable => \$autocmd, -command => sub { $docmdbtn->configure(-state => 'normal'); })->pack(-side => 'right');
	$options->Label(-text => "Auto-Update Command?")->pack(-side => 'right');
	$options->Label(-text => "Short or Long Flags?")->pack(-side => 'left');
	$options->Radiobutton(-text => "Short", -value => "1", -variable => \$sf)->pack(-side => 'left');
	$options->Radiobutton(-text => "Long", -value => "0", -variable => \$sf)->pack(-side => 'left');
	$options = $optwin->Frame(-relief => 'ridge', -borderwidth => 2)->pack(-fill => 'x', -side => 'left');
	$options->Radiobutton(-text => "Lev. 3", -value => "3", -variable => \$verbose, -command => sub { &do_command })->pack(-side => 'right');
	$options->Radiobutton(-text => "Lev. 2", -value => "2", -variable => \$verbose, -command => sub { &do_command })->pack(-side => 'right');
	$options->Radiobutton(-text => "Lev. 1", -value => "1", -variable => \$verbose, -command => sub { &do_command })->pack(-side => 'right');
	$options->Radiobutton(-text => "No", -value => "0", -variable => \$verbose, -command => sub { &do_command })->pack(-side => 'right');
	$options->Label(-text => "Verbose?")->pack(-side => 'right');
	$options = $optwin->Frame(-relief => 'ridge', -borderwidth => 2)->pack(-fill => 'x', -side => 'right');
	$options->Radiobutton(-text => "STDOUT", -value => "1", -variable => \$msgs, -command => sub { select(STDOUT); })->pack(-side => 'right');
	$options->Radiobutton(-text => "Log File", -value => "0", -variable => \$msgs, -command => sub {
		my $temp = $ENV{"HOME"} . "/.rGUIlog";
		if(open(LOGFILE,">$temp")) { select(LOGFILE); } else { print STDOUT "Couldn't open $temp, resorting to STDOUT"; select(STDOUT); $msgs = 0; }
	})->pack(-side => 'right');
	$options->Label(-text => "Message output?")->pack(-side => 'right');
}

sub save_prefs {
	my $temp = $ENV{"HOME"} . "/.rGUIprefs";
	if (open(FILE,">$temp")) {
		print FILE "BIN=$bindir\n";
		print FILE "FLAGS=$sf\n";
		print FILE "VERBOSE=$verbose\n";
		print FILE "AUTOCMD=$autocmd\n";
		print FILE "MSGS=$msgs\n";
		close(FILE);
	} else { print "Cannot create preferences file!\n"; }
}

