#include <iostream>
#include <vector>
#include <stack>

using namespace std;

class Solution {
public:
    //先用栈做一次。熟悉一下这个做法
    //想法是没错的。。。找到一个比当前栈顶大的柱子。然后往回pop去，一个一个把积攒的水量加起来。。
    //不过在实现的时候。。没写对。。导致代码无法实现自己的想法。。头疼啊
    //看了看答案去理解一下怎么实现。。

    /*
     * 发现了自己在开始上的一个失误。。。看例子，假设有这么一组柱子 2,1,0,1,3
     *     x
     * x···x
     * xx·xx
     * 21013
     * 01234
     * 我开始的想法是直接走到了3，然后3比1大对吧？？那就继续往前看。直到走到一个不比3小的，(即 >= 3)，或者走到了头这意味着这中间有一部分有可能可以承载一些水。。。
     * 然后我就开始直接写了。。后果就是错了。。问题出在哪？？
     * ！我只是关注了从3开始找到了2，然后算中间有多少个block啊加起来然后减一减就行。。并且我也是到>=3停止，也就是左边找到一个跟当前柱子一样高的时候也会停止
     * (因为一样高也可以承载水不是么，如果中间有空隙。。)
     *
     * 但是看这个例子。。。到1 - 0 - 1的时候。。第二个1那部分，就已经把中间这个'·'也就是水给算给算好了，然后我又算了一遍从3开始到2的。。又算了一遍这个水滴。
     * 无论如何。。结果肯定就不对了。。所以这里的问题是。。如何处理掉101这部分。
     * 说一下自己开始的错误处理方式吧。。。
     * 1、我想着那就可劲往左找，小于等于的也算上。。。然后发现这明显不行啊。。。比如 10303这种。。那我就跳过中间303直接到1了。。肯定结果就不对了。。
     * 2、按照不小于的正常逻辑往前找。。。到还剩1个2的时候停下来。。(暂且不管怎么停下来，事实上那会我也没想过这个问题。。)，就光这个逻辑。。就又把101多算了一次。。
     * 甚至变成了 101算一次，1013算一次。。21013再算一次。。直接炸裂。。。
     *
     * 所以看了看答案，重新分析一下。这波要怎么做。
     * 1、首先，找到 >= 当前柱子这个没错，上面说了 两个相同的可以一起处理。比如 303就可以算进去
     * 2、那么21013这种怎么办？？或者说 2103这种怎么办？我们先画出2103的图(2013不行。。跟我错误的那个思维符合要求。。还是21013把。。)
     * 那就继续21013。然后当我们找到了3怎么办？首先我们知道。 101中间的水已经被算了(别管算出来结果是啥。。但是肯定算了。。而且0这个位置的index2也已经pop出去了。)
     *    那么对于当前的(4,3)我们看到的是(3,1) 这俩之间没距离，所以是0。
     *    然后就继续是(1,1)，这里我们因为已经碰到个(3,1)了所以我们知道这里这个(1,1)没用。没用的原因就是前面碰到了1。然后按照我们的逻辑，比前面1小的都被pop了。
     *    剩的都是>=1的。那么肯定都是处理过存储水的。所以这个1不用处理。
     *    那么就继续下一个2。
     *    我们发现2可以用。但是不能全用。因为101被处理过了。我们只能处理2的那部分。相当于此时是一个 10002这样子。
     *    (这部分其实是最难想到的，对于我来说。。我总是局限在想一次算完所有的东西。。然后总数忽略了中间的细节。。导致少算这个情况。少算那个情况。。真的头疼。。)
     *    说回来为什么能当成10002这个样子去处理。
     *    ·首先，2比1高，这是必然的，根据我们的存储逻辑。剩的都是>=1的，然后2高出了1一截，那么就能在更高出比1存储更多的水，因为2比当前的柱子3小。
     *    所以区别就在于，要把这个1给剪掉，1已经算过了(无论是之前提前算过还是没提前算过，下面画个图进行理解),也就是 2 - 1。
     *
     *    (先不画图，再打几句话)
     *    其实从stack的存储处理逻辑，不从代码出发，我们的目的是啥？
     *    1、找到一个比之前存储的高的柱子
     *    2、把这之间的水存好。再往前找一个根
     *    3、如果还有的话，直到找到那根比当前高的。这个比较显然，找到比当前高的，那之前的水就是由那个更高的来决定了。跟当前的就没关系了。
     *    所以这里就是。如果我们从当前开始往前找，找到一根，那么就是把这根高度等级上的都给处理了比如， 1003，我们处理的是等级1的
     *    然后再碰到2，那我们处理的是21003。然后我们发现1这个等级已经被处理了。所以要把1剪掉。来处理剩余的部分。这么一说其实就比较清晰了。
     *          处理完小的
     *          拿出更高的
     *          把小的处理过的部分去掉
     *          处理剩余的部分
     *          以此类推
     *    那会不会多算呢？我们来画个图(终于画图了。。23333)
     *          x
     *    x·····x
     *    x·x···x
     *    x·x·x·x
     *    3020104
     *    看这种，我脑海中第一反应就是。。这么遇到4会不会就少算了？？其实这么一想就不会。因为
     *    1、在遇到4之前，2已经碰到过0，处理过302
     *    2、1碰到过1，处理完201
     *    3、那么在碰到4的时候，我们先处理了104，再处理20104的时候，201，104的部分都ok了，剩余的就是2 - 1后根4的部分了
     *    4、然后处理3020104的时候，20104被处理了，302被处理了。剩余的就是3-2后剩余的部分了。
     *    所以我们综上发现，只要找到这个一根比当前的高的。就可以开始进行计算，并且往前找到任一一个比当前柱子小的柱子，把自己这个高度等级的水处理完。
     *    然后我们发现。。302，，这个2比0大但是比3小。最终是用2来做高度处理的。
     *    这就是刚刚的条件了。直到找到一个 >=当前柱子的，那么就可以把这之间的，比这两个柱子都小的柱子等级都处理掉。对于后续的更高的柱子，这些等级的前面的处理完了。只要处理后续的
     *    差别出来的等级就行了。
     *    (说这么多，还不是因为看了答案。。才知道要这么做。。真的是。。开始想的时候这个21013太天真的想法把自己给绕进去了。。然后半天还绕不出来。。
     *    最后才发现，这个柱子高度等级的处理，才是关键。。哎。再多想想。。多理解一下把。。2333)
     *
     *    噢对。所以这里的逻辑是，202这种。找到2处理完后。还要继续往前找的。这种不会出错的原因就是，如果前面有比2高的。那没错。我们处理完2的高度等级，处理去掉2的部分
     *    如果前面有比2低的。。也早都处理完了。所以没差。
     *    ！！！！不指望自己能记住上面这些。。但是思考了就是王道。23333
     *
     *
     */
    //先用栈做一次。熟悉一下这个做法
    //想法是没错的。。。找到一个比当前栈顶大的柱子。然后往回pop去，一个一个把积攒的水量加起来。。
    //不过在实现的时候。。没写对。。导致代码无法实现自己的想法。。头疼啊
    //看了看答案去理解一下怎么实现。。
    int trap(vector<int>& height) {
        stack<int> s;
        int n = height.size();
        int i = 0;
        int res = 0;
        int cur_left, w, h;
        while (i < n) {
            int cur_level = 0;
            while(!s.empty() && height[s.top()] <= height[i]) {
                cur_left= s.top();
                s.pop();
                w = i - cur_left - 1;
                //这里用min是因为我们存在 302 203两种
                //302是我们找到了一个比当前高的
                //203是最终会一路找下去的。。
                //我们在处理当前柱子等级的时候，肯定是低的那个。而低的那个不知道是当前柱子，还是最终找到的柱子，因为有两个情况。所以取个min
                h = min(height[cur_left], height[i]) - cur_level;
                res += w * h;
                cur_level = height[cur_left];
            }
            //这步不能少。。因为上面条件要么是空(也就是之前是203 303这种)，要么是大于(也即是403这种)
            //403这种到4就停止了。。没算上。所以要下面判断一下是因为啥结束的。。如果栈不空就意味着有个更高的。。要单独处理一下。。
            if (!s.empty()) {
                w = i - s.top() - 1;
                h = min(height[s.top()], height[i]) - cur_level;
                res += w * h;
            }
            s.push(i);
            i++;
        }
        return res;
    }


    //再换一下所谓的dp(我都不知道为啥这个算dp。。干)
    /*
     * dp的出发点都是从brute-force来，然后通过记录一些东西。来减少多次的重复使用。。。
     * 然后我发现。。这个题我连brute-force都不会。。我靠。。
     * 然后我就开始研究brute-force做法了。。
     *
     * brute-force居然是。。。
     * 看每一个柱子能够承载多少水。。我靠。。这也太难了。。我没想到啊。。。比如
     * 1312这种
     * 1头上就不能，3不能，第二1头上可以承载1，因为左3右2。。。。2不能
     * 然后算出来是1。。我靠。。我惊呆了。。
     * 这样子算出来所有被两根 >=当前柱子包裹的柱子所能承载的水，就得到结果了(所以是。。这么来的？？刚好相同高度就是h-h=0了。。我去)
     *
     * 所以这里才诞生了dp。。。
     * dp就是。。当我对这个柱子只往左边看，假设右边是一个无限高的柱子。。那么我就能得到这个柱子的可能承载水量
     * 然后我再重来一遍往右边看，假设左边是一个无限高的柱子，，那么就也能得到一个可能的承载水量。
     * 很明显。。两个中小的那个才是我们要的。。
     *
     * (先不提我自己能不能想到这个办法。。。TM我之前做的时候。我还以为dp是来记录这个柱子和左边的、或者是右边的包裹面积。。卧槽？？我以前都是咋想的。。
     * 果然人会成长么？？？233333)
     *
     *
     * 那么就来分析，为什么这么做是对的。
     * 要从brute-force分析。。。
     * 我们是要去一个柱子左右找到有可能 >= 这个柱子高度的两根柱子，来算这个柱子上能承载多少水。也就是说
     * 1、如果我能找到这么两根柱子，那么取小的那个，根当前的做减法，就能得到一个结果。
     * 2、如果我其中一边找不到，那么必然这个柱子上就是0了。
     * 3、那么我们就可以根据2，把1拆掉。我们先找一边
     *      找到了？那么记录下来，很有可能未来这个找到的柱子有大用，能帮当前柱子承载水
     *      找不到？那么很明显，这个柱子以后也不会有水在上面了。
     * 所以综上，brute-force找的没错。就是处理的多余了点。因为当一边找不到，另一边就没必要了。
     * 同理当两边都能找到，取小的就行。
     * 所以总结到最后就成了。先找一边的，看看每根柱子的情况。再找另一边的，看看每根柱子的情况。然后综合一下。就有结果了。
     *
     * 然后问题来了。
     * 那这岂不是从每个柱子往左或者往右一直找比他大的么？？，岂不是比以前的n^2再乘个2，更慢了？？
     * 所以这里要明白，啥叫走一遍。
     * 比如我们从左边开始每根柱子只关注他左边有没有比他高的。那简单了，我从左边开始记录能找到的最大的。。按顺序执行。那么只要当前柱子比这个最大的大，我就更新就行了。
     * 反正我是从0开始按顺序来的。不会存在漏找情况的。。
     * 这样子之后，我们就能知道，对于每根柱子，如果只看他的左边，它能承载多少水(即是右边无限高的墙挡着)
     * 同理从右边再来一遍就行。然后再走一遍综合一下就行。
     *
     * 这里dp缩减时间所用到的方法就是。在brute-force中，我们不断的去当前柱子的左边或者右边找最大的柱子。然后事实上，对于一根柱子，例如，当它找完左边的最大柱子后，
     * 下一根柱子没有必要重新走一遍，，他只要看自己和当前左边已经找完的那个最大的谁大就行了。
     * 同理右边。
     * 就是这样子省去了查询时间。dp的效果就达到了。
     * (反正我就是事后诸葛亮！！！知道做法了再分析呗。。以后能想到算自己牛逼。想不到拉到。GG)
     */

    int trap(vector<int>& height) {
        int n = height.size();
        if(!n) return 0;
        vector<int> left(n, 0), right(n, 0);

        //左到右，看左，右无限大
        int left_max = height[0];
        for (int i = 1; i < n; i++) {
            if (height[i] < left_max) {
                left[i] = left_max - height[i];
            }
            else {
                left_max = height[i];
            }
        }
        int right_max = height[n - 1];
        for (int i = n - 2; i >= 0; i--) {
            if (height[i] < right_max) {
                right[i] = right_max - height[i];
            }
            else {
                right_max = height[i];
            }
        }
        int res = 0;
        for (int i = 0; i < n; i++) {
            res += min(left[i], right[i]);
        }
        return res;
    }

    /*
     * 上面那个写法还是没领悟到精髓。。。我用一个变量存了当前左边最大的柱子。。其实刚刚上面分析了。。dp提速的原因是在于
     * 每个柱子都知道上一个柱子算后最大的柱子是多少。。所以dp中存柱子才是正确做法。。(所谓的符合dp的提速逻辑做法？？)
     * 否则代码读起来怪怪的。分析逻辑也怪怪的。。23333
     * 所以改了一下。
     * 下面就比较符合从 brute-force提速到dp的逻辑了。。
     */
    int trap(vector<int>& height) {
        int n = height.size();
        if(!n) return 0;
        vector<int> left(n, 0), right(n, 0);

        //左到右，看左，右无限大
        left[0] = height[0];
        for (int i = 1; i < n; i++) {
            left[i] = max(left[i - 1], height[i]);
        }
        //右到左，看右，左无限大
        right[n - 1] = height[n - 1];
        for (int i = n - 2; i >= 0; i--) {
            right[i] = max(right[i + 1], height[i]);
        }
        //综合一下
        int res = 0;
        for (int i = 0; i < n; i++) {
            res += min(left[i], right[i]) - height[i];
        }
        return res;
    }

    /*
     * 然后就是高级的two pointers了。。。emmmm
     *
     * 这边先写分析
     * 从上面我们发现，我们在dp的时候。
     * 左到右可以用一个变量来存左边最大。。
     * 右到左可以用一个变量存右边最大。。
     * 然后结果的时候，我们根据当前柱子存的左右小的那个，来决定最终承载水量是多少。
     *
     * 那么这里我们就有一个想法了。。
     * 对于当前的柱子，当前的左右两个最大。是不是只要这两个最大，比这个柱子高，这个柱子就能承载水？
     * 是的。这是由brute-force得到的结果。
     * 所以我们能不能用two pointers 左右两边一起走，一起找最大的去做？
     * 也是可以的。
     * 那么逻辑是什么？
     * 1、我们可以首先固定两边最远端的柱子(最左和最右)，这就是当前最大左，最大右。即，0，n-1
     * 2、现在问题来了。我们有两个指针i, j。我们动i还是动j？？？i j代表当前我们要处理的两根柱子。
     *    所谓的动i还是动j就是先处理i还是先处理j。处理完后就可以i++, j--。也就是动i还是动j了。
     *    这个时候就要从上面的逻辑本身出发了。。
     *    对于处理i或者j，我们希望的是看i或者j对于left_max, right_max的大小关系。
     *    如果i 在这俩之间。就意味着i可以承载>=0的水。同理对j
     *
     *    然后当我们处理完i或者j后。我们还要更新left_max 或者 right_max。
     *
     *    所以很明显，我们应该处理i j 中对应柱子低的那个。
     *    1、因为低的那个才有可能对于当前的两根左右最大的柱子来承载水量。
     *    2、如果选高的那个。。很有可能最后。某个低的这辈子都无法选中了。。就少了。。(毕竟最高的那个可以不承载水，算不算无所谓。。但是最低的那个很大概率会承载水。)
     *    3、最重要的(其实这点才是最关键的。。1和2都是。。我凭感觉来的。。。)
     *       如果我们每次都选高的那个处理。那么处理完高的那个后，高的那个马上就会进入下一个位置。如果下一个位置比上一个矮但是比另外一边当前的高且比下一个矮呢？？(很绕是吧，我自己打字都绕了。2333)
     *       其实就是如果我们每次都选高的话。高的那边一直更新。。矮的那边永远不动。那么本来中间有根柱子左右是可以满足条件的，但是我们一直在更新高的那个，
     *       然后走到了这根本来可以满足条件的柱子。。结果因为另一边矮的没动，程序就以为，那边还很矮啊。。这边不能承载水啊。。就炸了。。
     *
     *       然后其实最后我们会发现，ij会汇聚到中间最高的那根柱子上。。。逻辑就是这么来的呗。。。
     *
     *   综上就是。在更新的时候，从短的柱子更新，这个样子后续的柱子才能正常的判断承载水的可能性，否则从长的更新，短的永远不动，作为一个木桶，短板才是最致命的。。长的更新了多长
     *   都无法弥补短的失误。。。然后中间本来能承载水的部分就都没用了。。炸裂。。。。
     *
     *
     *
     */

    /*
     * 。。感觉大脑是宕机了。。。或者说有个地方绕不出来。。。。
     * 刚刚脑补了一波理解。。感觉就这样了。。再想就死在这里了。2333
     *
     * 为啥ij处理选小的那个，很简单的一个想法就是。我们希望从小的柱子开始更新。只有先更新小的柱子，才能让中间的柱子有可能承载更多的水。
     *
     * 那为什么每次i只要根左边比，j只要根右边比呢？？
     * 原因就在于。我们在更新逻辑上，永远先选i j中小的那个进行更新。也就意味着，当顺着个这个逻辑往下，说个流程
     * i = 0 j = 7. 数组H
     * H[0] < H[7]
     * 我们更新i到1
     * H[1] > H[7]
     * 我们更新j到6
     * 我们发现。在这个逻辑下。当你更新某边时，另一边当前存留的最大就一定是比这边小的。
     * 因为逻辑上就是Hi 比 Hj小，我才更新i这边。直到Hi比Hj大，我才去j那边。那么去j那边的时候我们就知道了，来这边的原因就是因为。。。我们找到了一根比hj更大的。。
     * 保证在hi这边有足够大的柱子可以让hj去承载水量。那么hj只要根右边比就行了。
     *
     * 所以说谁小更新谁，更新完后他有可能更大，更大的话才能让之前另一边大的有可能变小，从而承载水量。。
     * 总而言之就是。。桶子要从短的来就明白了。。
     *
     *
     * (我突然就明白了。。草啊！！！！！！好呆啊)
     * 我总是在想，当前柱子必须跟两边最高柱子都有关系啥的。。然后才能处理。。。
     * 但是我完全忽略了。。height[i] < height[j]这个问题了。
     * 这不就是在j那边找了一个比i高的柱子么？？
     * 那不就是直接满足条件了么？？我靠。。
     * 所以整个的逻辑是。。。
     * 如果height[i] < height[j]这种一直跑，就意味着。对于这边的i那边有个贼高的j。能够让这边的i都可能承载水。至于能否真正的承载水。要看left_max
     * 同理对右边一样。height[j] < height[i]。 就是左边找到一个贼高的i，让右边的j都可能承载水，具体是否能够承载，还要看right_max
     * 这么一想立马就懂了。。
     *
     * 相当于就是 对左边的i，如果右边找不到更高的j，就继续j--找更高的j。同时j也有了更高的i。。刚好一起处理。。
     * 然后对于右边的j。如果左边找不到更高的i，就继续i++找更高的i。同时i也有了更高的j。。也是一起处理了。。
     * 卧槽。。。想明白了。。干！！！！
     *
     * 经历刚刚最后写的那个，。。下面那个人的反而不好理解了。。23333
     * 下面有另一个人的代码，更好理解一点。
     */
    int trap(vector<int>& height) {
        int n = height.size();
        if(!n) return 0;
        int left_max = 0;
        int right_max = 0;
        int res = 0;
        int i = 0, j = n - 1;
        while (i < j) {
            if (height[i] < height[j]) {
                if (height[i] < left_max) {
                    res += left_max - height[i];
                }
                else {
                    left_max = height[i];
                }
                i++;
            }
            else {
                if (height[j] < right_max) {
                    res += right_max - height[j];
                }
                else {
                    right_max = height[j];
                }
                j--;
            }
        }
        return res;
    }

    /*
     * 这个就是two pointer的另一种写法。这个更能符合我的理解。。更明显吧。。
     * 就我纠结的是。碰到ij的时候。动哪个？？？
     * 1、首先碰到一个i j我们需要更新当前两边最大。这个不能漏。。因为当我们到了一个新的i或者j的时候，这个i或者j可能比之前的更高。所以一定要更新一下。
     * 2、那动i还是动j。这就是我们上面遵循的道理。我们希望把短的柱子变的更高，所以比较左右最大，让短的那边动。如果是左，那就i，反之j。
     *
     * 这里直接举例子为啥不能从大的。。。
     * 首先上面说了很多了。。如果从大的动。。你从大的 如果变成次大，然后这个次大本该在另一边动完后可以承载水，但是因为没有把短的动了。就到这个次大了。。就炸了。例子如下
     *         x
     *       x x
     *   x   x x
     *   x x x x
     * 0 2 1 3 4
     *
     * 本来1可以承载水。但是我们如果从大的。也就是4更新。。更完到3。更完后到1。。。这个时候左边的最大还是0。。。压根就没动。。我们只是在不断的动右边大的那个柱子
     * 小的一直没动。。那压根就不可能让中间的柱子上承载水了。。。
     * 所以。。没啥可纠结的。。这么一看就知道了。。先让短的变长。。让另一边本来长的有可能承载水的才有机会处理。。。
     */

    int trap(vector<int>& height) {
        int n = height.size();
        if(!n) return 0;
        int left_max = 0;
        int right_max = 0;
        int res = 0;
        int i = 0, j = n - 1;
        while (i < j) {
            left_max = max(left_max, height[i]);
            right_max = max(right_max, height[j]);
            if (left_max < right_max) {
                if (height[i] < left_max) {
                    res += left_max - height[i];
                }
                i++;
            }
            else {
                if (height[j] < right_max) {
                    res += right_max - height[j];
                }
                j--;
            }
        }
        return res;
    }
};

/*
 * 写了一堆。。能想到的第一次就是栈的做法。。然后问题是我还做错了。。就TM很气。。。DP那个在理解了brute-force后就释然了。
 * two pointer那个还是从dp的我自己的冗余写法下让我更好理解延伸过来了。。我惊呆了。。。但是在处理逻辑上真的就墨迹了很久。。感觉脑子不太够用了。。哎。。
 */