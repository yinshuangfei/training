<title>天玥系列磁盘阵列控制台</title>
<link rel="stylesheet" href="assets/css/jquery-ui.min.css" />
<script type="text/javascript" src="assets/js/jquery1x.min.js"></script>
<script src="assets/js/echarts/echarts.min.js"></script>

<div class="table-header"></div>
<div id="main" style="width:600px; height:400px;"></div>

<script type="text/javascript">

    var data_update;
    // This is for dark theme
    // var myChart = echarts.init(document.getElementById('main'), 'dark');
    var myChart = echarts.init(document.getElementById('main'));

    myChart.setOption({
        title: {
            text: '流量统计'
        },
        tooltip: {},
        legend: {
            data:['Sales']
        },
        xAxis: {
            data: []
        },
        yAxis: {
            type: 'value'
        },
        series: [{
            name: 'MB/s',
            type: 'line',
            areaStyle: {
                normal: {}
            },
            data: []
        }]
    });
    
    function addData(shift) {
        $.get('config/traffic/u1_point.php').done(function (mydata){
            data_update = mydata;
        });
        console.log(data_update);

        // var now = new Date();
        // data_update.time.push(now);
        // data_update.write.push((Math.random()) * 100000);

        // if (shift) {
        //     data_update.time.shift();
        //     data_update.write.shift();
        // }
    }
    $.ajax({  
        url: 'config/traffic/u1_point.php',
        type: 'GET',
        contentType: 'application/json; charset=UTF-8',
        success: function(response) {
            console.log('response');
        },
        error: function(jqXHR, textStatus, errorThrown) {
            console.log('textStatus');
        }
            
      });
    // $.get('config/traffic/u1_point.php').done(function (data) {
    //     // fill in data
    //     console.log(data);

    //     data_update = data ;
        
    //     myChart.setOption({
    //         xAxis: {
    //             data: data.time
    //         },
    //         series: [{
    //             // find series by name
    //             name: 'Sales',
    //             data: data.write
    //         }]
    //     });

    //     // set the interval funtion
    //     setInterval(function () {
    //         // addData(true);
    //         myChart.setOption({
    //             xAxis: {
    //                 data: data_update.time
    //             },
    //             series: [{
    //                 data: data_update.write
    //             }]
    //         });
    //         console.log("hello");
    //     }, 1000);
    // });
</script>