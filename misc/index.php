<html>
<head>
<title><?php echo getcwd(); ?></title>
<style type='text/css'>
div.pic { 
    display: block;
    float: left;
    border: 1px solid gray;
    margin: 3px;
}
</style>
</head>
<body>
<h1><?php echo getcwd(); ?></h1>
<h2><a name="plots">Plots</a></h2>
<p><form>Filter: <input type="text" name="match" size="30" value="<?php if (isset($_GET['match'])) print htmlspecialchars($_GET['match']);  ?>" /><input type="Submit" value="Go" /></form></p>
<div>
<?php
if ($_GET['noplots']) {
    print "Plots will not be displayed.\n";
} else {
    $other_exts = array('.pdf', '.cxx', '.eps');
    foreach (glob("*.png") as $filename) {
        if (isset($_GET['match']) && !fnmatch('*'.$_GET['match'].'*', $filename)) continue;
        print "<div class='pic'>\n";
        print "<h3><a href=\"$filename\">$filename</a></h3>";
        print "<a href=\"$filename\"><img src=\"$filename\" style=\"border: none; width: 300px; \"></a>";
        $others = array();
        foreach ($other_exts as $ex) {
            if (file_exists(str_replace('.png', $ex, $filename))) {
                array_push($others, "<a class=\"file\" href=\"".str_replace('.png', $ex, $filename)."\">[" . $ex . "]</a>");
            }
        }
        if ($others) print "<p>Also as ".implode(', ',$others)."</p>";
        print "</div>";
    }
}
?>
</div>
<div style="display: block; clear:both;">
<h2><a name="files">Other files</a></h2>
<ul>
<?
foreach (glob("*") as $filename) {
    if ($_GET['noplots'] || !preg_match('/.*\.png$/', $filename)) {
        print "<li><a href=\"$filename\">$filename</a></li>";
    }
}
?>
</ul>
</div>
</body>
</html>
